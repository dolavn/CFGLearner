import xlrd
import json
import csv
from nltk import Tree
from nltk.draw.util import CanvasFrame
from nltk.draw import TreeWidget


def draw_tree_to_file(tree, path):
    cf = CanvasFrame()
    tc = TreeWidget(cf.canvas(), tree)
    x, y = 0, 0
    cf.add_widget(tc, x, y)
    cf.print_to_file(path)
    cf.destroy()


def parseTree(string):
    print(string)
    root = Tree(0, [])
    stack = [root]
    curr_gene = ''
    for c in string:
        if c == '(':
            stack.append(Tree(0, []))
            continue
        if c == ',':
            curr_tree = stack[len(stack)-1]
            curr_tree.insert(len(curr_tree), Tree(curr_gene, []))
            curr_gene = ''
            continue
        if c == ')':
            curr_tree = stack.pop()
            father = stack[len(stack)-1]
            if curr_gene != '':
                curr_tree.insert(len(curr_tree), Tree(curr_gene, []))
            if len(curr_tree) > 0:
                father.insert(len(father), curr_tree)
            curr_gene = ''
            continue
        curr_gene = curr_gene + c
    root = stack.pop()[0]
    if len(stack) > 0:
        raise BaseException("Invalid string")
    return root


def read_xls(instance_dict=None):
    TREES_OUTPUT = False
    KEEP_STRAND_INFO = False
    fname = 'dataset_relabeled_strand_aligned_by_MF_TF.xlsx'
    fname = 'atp_ion_transport.xlsx'
    lst = []
    xl_workbook = xlrd.open_workbook(fname)
    xl_sheet = xl_workbook.sheet_by_index(0)
    row = xl_sheet.row(0)
    id_idx = 0
    count_idx = 3
    csb_idx = 4
    tree_idx = 5
    for row_idx in range(1, xl_sheet.nrows):
        tree_obj = xl_sheet.cell(row_idx, tree_idx)
        csb_obj = xl_sheet.cell(row_idx, csb_idx)
        if csb_obj.value == 'NONE':
            continue
        id_obj = xl_sheet.cell(row_idx, id_idx).value
        if id_obj == 'NONE':
            continue
        count_obj = xl_sheet.cell(row_idx, count_idx)
        value = csb_obj.value
        if instance_dict is None:
            count = int(count_obj.value)
        else:
            count = instance_dict[id_obj]
        if value == 'NONE':
            continue
        value = value.replace('[', '(')
        value = value.replace(']', ')')
        elem = None
        if TREES_OUTPUT:
            tree = parseTree(value)
            draw_tree_to_file(tree, 'tree_{}.ps'.format(row_idx))
            elem = str(tree)
        else:
            revFlag = '-' in value
            value = value.replace('(', '')
            value = value.replace(')', '')
            if not KEEP_STRAND_INFO:
                value = value.replace('+', '')
                value = value.replace('-', '')
            elem = value.split(',')
            if revFlag:
                elem = elem[::-1]
        lst.append((elem, count))

    print(lst)
    output = 'michalTrees' if TREES_OUTPUT else 'michalAtpIonStrings'

    with open(output, 'w') as jsonOutput:
        json.dump(lst, jsonOutput)


def read_instances():
    PATH = 'instances.txt'
    ans = {}
    curr_cog = None
    with open(PATH) as file:
        for line in file:
            if line[0] == '>':
                line = line[1:].split('\t')
                curr_cog = line[0]
                continue
            if curr_cog is None:
                raise BaseException("Invalid file")
            line = line.split('\t')[0]
            line = line.split('_')[:2]
            line = '_'.join(line)
            if curr_cog in ans:
                if line in ans[curr_cog]:
                    continue
                ans[curr_cog].append(line)
            else:
                ans[curr_cog] = [line]
    for cog in ans.keys():
        ans[cog] = len(ans[cog])
    return ans


instance_count = read_instances()
print(instance_count)
read_xls(instance_dict=instance_count)