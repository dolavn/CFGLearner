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


def get_least_frequent_cogs(csb_list, k=5):
    alphabet = {}
    for l, occ in csb_list:
        curr_lst = []
        for c in l:
            curr_lst.append(c)
        curr_lst = set(curr_lst)
        for c in curr_lst:
            if c not in alphabet:
                alphabet[c] = occ
            else:
                alphabet[c] = alphabet[c] + occ
    sorted_set = sorted([(k, alphabet[k]) for k in alphabet.keys()], key=lambda a: a[1])
    print(len(sorted_set))
    return [cog[0] for cog in sorted_set[:k]]


def remove_csbs_with_cogs(csb_list, cog_list):
    return list(filter(lambda csb: not any([cog in cog_list for cog in csb[0]]), csb_list))


def keep_csbs_with_cogs(csb_list, cog_list):
    return list(filter(lambda csb: all([cog in cog_list for cog in csb[0]]), csb_list))


def read_xls(instance_dict=None):
    TREES_OUTPUT = False
    KEEP_STRAND_INFO = False
    fname = 'dataset_relabeled_strand_aligned_by_MF_TF.xlsx'
    fname = 'atp_ion_transport.xlsx'
    fname = 'dataset.xlsx'
    fname = 'test_data2.xlsx'
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
        lst.append((elem, id_obj))
    cog_list = ['COG5002', 'COG0745', 'COG0704', 'COG1117', 'COG0581', 'COG0573', 'COG0226']
    cog_names = {'COG5002': 'VicK', 'COG0745': 'OmpR', 'COG0704': 'PhoU', 'COG1117': 'PstB',
                 'COG0581': 'PstA', 'COG0573': 'PstC', 'COG0226': 'PstS'}
    #lst = keep_csbs_with_cogs(lst, cog_list)
    for l in lst:
        for i, e in enumerate(l[0]):
            if e in cog_names:
                l[0][i] = cog_names[e]
            else:
                l[0][i] = 'UNKNOWN'
    #lst = list(filter(lambda a: a[1] >= 10, lst))
    output = 'michalTrees' if TREES_OUTPUT else 'michalTest'
    print(lst)
    with open(output, 'w') as jsonOutput:
        json.dump(lst, jsonOutput)


def read_instances():
    PATH = 'test_data_instances.fasta'
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
read_xls(instance_dict=None)