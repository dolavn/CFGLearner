import xlrd
import json
import csv
from nltk import Tree
from nltk.draw.util import CanvasFrame
from nltk.draw import TreeWidget
from itertools import product
import copy
import sys

KEEP_STRAND_INFO = False
MUST_INCLUDE = ['COG0583']

with open('fim_cogs', 'r') as cog_file:
    COG_LIST, COG_NAMES, ANNOT_DICT = json.load(cog_file)

print(COG_NAMES)
"""
COG_LIST.append('COG0583')
COG_LIST.append('COG1846')
COG_LIST.append('COG0642')
COG_LIST.append('COG0745')
COG_NAMES['COG0583'] = 'LysR'
COG_NAMES['COG1846'] = 'MarR'
COG_NAMES['COG0642'] = 'BaeS'
COG_NAMES['COG0745'] = 'OmpR'
print(COG_LIST)
print(COG_NAMES)
ANNOT_DICT = COG_NAMES
COG_LIST.remove('COG1289')
del(COG_NAMES['COG1289'])
del(ANNOT_DICT['COG1289'])

"""


def add_suff(inp, to_add):
    if len([c for c in inp if c=='.']) != 1:
        raise BaseException("Invalid input")
    dot_ind = inp.index('.')
    return inp[:dot_ind] + to_add + inp[dot_ind:]


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
    sorted_set = sorted([(k, alphabet[k]) for k in alphabet.keys()], key=lambda a: a['seq'])
    print(len(sorted_set))
    return [cog[0] for cog in sorted_set[:k]]


def remove_csbs_with_cogs(csb_list, cog_list):
    return list(filter(lambda csb: not any([cog in cog_list for cog in csb['seq']]), csb_list))


def keep_csbs_with_cogs(csb_list, cog_list):
    return list(filter(lambda csb: all([cog in cog_list for cog in csb['seq']]), csb_list))


def remove_csbs_without_cogs(csb_list, cog_list):
    return list(filter(lambda csb: not all([cog not in cog_list for cog in csb['seq']]), csb_list))


ID_IDX = 0
COUNT_IDX = 3
CSB_IDX = 4
TREE_IDX = 5
TREES_OUTPUT = False


def read_row(xl_sheet, row_idx, instance_dict):
    csb_obj = xl_sheet.cell(row_idx, CSB_IDX)
    if csb_obj.value == 'NONE':
        return None
    id_obj = xl_sheet.cell(row_idx, ID_IDX).value
    if id_obj == 'NONE':
        return None
    count_obj = xl_sheet.cell(row_idx, COUNT_IDX)
    value = csb_obj.value
    if instance_dict is None:
        count = int(count_obj.value)
    else:
        count = instance_dict[id_obj]
    if value == 'NONE':
        return None
    value = value.replace('[', '(')
    value = value.replace(']', ')')
    elem = None
    if TREES_OUTPUT:
        tree = parseTree(value)
        draw_tree_to_file(tree, 'tree_{}.ps'.format(row_idx))
        elem = str(tree)
    else:
        neg_num, pos_num = [len([c for c in value if c == q]) for q in ['-', '+']]
        value = value.replace('(', '')
        value = value.replace(')', '')
        if not KEEP_STRAND_INFO:
            value = value.replace('+', '')
            value = value.replace('-', '')
        elem = value.split(',')
        revFlag = neg_num > pos_num
        if revFlag:
            print(id_obj)
            elem = elem[::-1]
    return id_obj, elem, count_obj.value


def create_rows_list(xl_sheet, instance_dict):
    ans = []
    for row_idx in range(1, xl_sheet.nrows):
        row = read_row(xl_sheet, row_idx, instance_dict)
        if row is None:
            continue
        csb_id, elem, count = row
        ans.append({'csb_id': csb_id, 'seq': elem, 'instances': count})
    return ans


def create_unknowns(lst, masked=None):
    if masked is None:
        masked = []
    unknown_dict = {}
    curr_unknown = 0
    for l in lst:
        annotations = []
        for i, e in enumerate(l['seq']):
            if e not in masked and e in COG_NAMES:
                annotations.append(ANNOT_DICT[e])
                l['seq'][i] = COG_NAMES[e]
            else:
                if e not in unknown_dict:
                    unknown_dict[e] = curr_unknown
                    curr_unknown = curr_unknown + 1
                annotations.append('UNKNOWN{}'.format(unknown_dict[e]))
                l['seq'][i] = 'UNKNOWN{}'.format(unknown_dict[e])
        l['annotation'] = annotations
    return unknown_dict


def read_xls(path, output, instance_dict=None, save_unknown=False, mask_cog=None, keep_only=None):
    xl_workbook = xlrd.open_workbook(path)
    xl_sheet = xl_workbook.sheet_by_index(0)
    rows_list = create_rows_list(xl_sheet, instance_dict)
    #print(lst)
    if not save_unknown:
        rows_list = keep_csbs_with_cogs(rows_list, COG_LIST)
    rows_list = remove_csbs_with_cogs(rows_list, ['COG1289'])
    if mask_cog is not None:
        rows_list = remove_csbs_with_cogs(rows_list, mask_cog)
    if keep_only is not None:
        rows_list = remove_csbs_without_cogs(rows_list, keep_only)
    print(rows_list)
    #lst = list(filter(lambda a: a['instances'] >= 10, lst))
    unknown_dict = create_unknowns(rows_list, masked=keep_only)
    new_rows_lists = []
    if save_unknown:
        it = zip(COG_NAMES.values(), ANNOT_DICT.values())
        for tup in product(*[it]*len(unknown_dict)):
            new_rows_list = copy.deepcopy(rows_list)
            for l in new_rows_list:
                for i, e in enumerate(l['seq']):
                    if 'UNKNOWN' in e:
                        unknown_ind = int(e[-1])
                        l['seq'][i] = tup[unknown_ind][0]
                        l['annotation'][i] = tup[unknown_ind][1]
            new_rows_lists.append(new_rows_list)
    with open(output, 'w') as jsonOutput:
        json.dump(rows_list, jsonOutput)
    print(rows_list)
    for ind, new_lst in enumerate(new_rows_lists):
        with open('{}_copy{}.json'.format(output, ind), 'w') as jsonOutput:
            json.dump(new_lst, jsonOutput)


def read_instances(path):
    ans = {}
    curr_cog = None
    with open(path) as file:
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


def concat_duplications(seq):
    ans = []
    indices = []
    last_char = None
    last_ind = -1
    for ind, s in enumerate(seq):
        if s == last_char:
            continue
        if ind-last_ind > 1:
            indices.append((last_ind, ind))
        last_ind = ind
        last_char = s
        ans.append(s)
    if len(seq)-last_ind > 1:
        indices.append((last_ind, len(seq)))
    return tuple(ans), indices


def get_all_substrings(tup, min_len=0, max_len=None):
    if max_len is None:
        max_len = len(tup)
    for curr_len in range(min_len, max_len+1):
        for i in range(len(tup)-curr_len+1):
            yield tuple(tup[i: i+curr_len])


def read_all_csbs(path, keep_unknown=False, tag_unknown=False):
    ans = {}
    xl_workbook = xlrd.open_workbook(path)
    xl_sheet = xl_workbook.sheet_by_index(0)
    score_idx = 2
    csb_idx = 4
    for row_idx in range(1, xl_sheet.nrows):
        score_obj = xl_sheet.cell(row_idx, score_idx)
        csb_obj = xl_sheet.cell(row_idx, csb_idx)
        value = csb_obj.value
        if value == 'NONE':
            continue
        else:
            revFlag = '-' in value
            value = value.replace('(', '')
            value = value.replace(')', '')
            if not KEEP_STRAND_INFO:
                value = value.replace('+', '')
                value = value.replace('-', '')
            elem = value.split(',')
            if not keep_unknown and any([cog not in COG_NAMES for cog in elem]):
                #print('cont')
                print([cog for cog in elem if cog not in COG_NAMES])
                continue
            if revFlag:
                elem = elem[::-1]
            elem = ['UNKNOWN' if e not in ANNOT_DICT else ANNOT_DICT[e] for e in elem]
            elem, _ = concat_duplications(elem)
            elem = tuple(elem)
            for substr in get_all_substrings(elem, min_len=2):
                if substr in ans:
                    ans[substr] += score_obj.value
                else:
                    ans[substr] = score_obj.value
    return ans


def load_cog_dict(path):
    with open(path) as json_file:
        cog_dict = json.load(json_file)
    return cog_dict


if __name__ == '__main__':
    args = sys.argv
    if len(args) < 4:
        print('not enough arguments')
        exit()
    path = args[1]
    target = args[2]
    csb_scores_target = args[3]
    mask_cog = None if len(args) == 4 else args[4]
    print(mask_cog)
    csb_dict = read_all_csbs(path)
    csb_dict = [(tup, csb_dict[tup]) for tup in csb_dict]
    print(csb_dict)
    with open(csb_scores_target, 'w') as jsonOutput:
        json.dump(csb_dict, jsonOutput)
    #instance_count = read_instances('fimC_fimID/dataset_instances.fasta')
    #print(instance_count)
    if mask_cog is not None:
        mask_cog = [mask_cog]
        read_xls(path, add_suff(target, '_without{}'.format(mask_cog[0])), mask_cog=mask_cog)
        read_xls(path, add_suff(target, '_with{}'.format(mask_cog[0])), keep_only=mask_cog, save_unknown=True)
    else:
        read_xls(path, target)
