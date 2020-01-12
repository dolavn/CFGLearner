import tkinter
from tkinter import Listbox, Frame, Canvas,\
    Scrollbar, Label, VERTICAL, Checkbutton, IntVar, Button, Toplevel
from tkinter.font import Font
import json
import sys
from time import time
from nltk import Tree
from nltk import treetransforms
from nltk.draw.util import CanvasFrame
from nltk.draw import TreeWidget
from CFGLearner import SimpleTeacher, FrequencyTeacher, DifferenceTeacher, Teacher, learn, TreeComparator, \
    SimpleMultiplicityTeacher, learnMult, learnMultPos, set_verbose, SwapComparator, DifferenceMultiplicityTeacher, \
    LoggingLevel, TreeConstructor
from WCFG import load_trees_from_file, convert_pmta_to_pcfg
from test import draw_trees
from threading import Thread


class Checklist(Frame):

    def FrameWidth(self, event):
        canvas_width = event.width
        self.canvas.itemconfig(self.canvas_frame, width = canvas_width)

    def OnFrameConfigure(self, event):
        self.canvas.config(scrollregion=self.canvas.bbox("all"))

    def __init__(self, elements, *args, **kwargs):
        Frame.__init__(self, *args, **kwargs)
        self.elements = elements
        self.elem_bools = [IntVar() for elem in elements]
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.checkbuttons = []
        self.selected_all = IntVar()

        sensorsFrame = Frame(self)
        sensorsFrame.grid(row=0, sticky="nsew")
        sensorsFrame.grid_rowconfigure(0, weight=1)
        sensorsFrame.grid_columnconfigure(0, weight=1)

        self.canvas = Canvas(sensorsFrame, bg="blue")
        self.sensorsStatsFrame = Frame(self.canvas)
        self.canvas.grid_columnconfigure(0, weight=1)
        self.sensorsStatsFrame.grid_columnconfigure(0, weight=1)

        myscrollbar = Scrollbar(sensorsFrame, orient=VERTICAL, command=self.canvas.yview)
        self.canvas.configure(yscrollcommand=myscrollbar.set)
        self.canvas.grid(column=0, sticky="nsew")
        myscrollbar.grid(row=0, column=1, sticky="nsew")
        for ind, elem in enumerate(elements):
            b = Checkbutton(self.sensorsStatsFrame, text="Tree {}".format(elem),
                            var=self.elem_bools[ind])
            b.grid(column=0, row=ind+1)
            self.checkbuttons.append(b)


        all = Checkbutton(self.sensorsStatsFrame, text="Select all", command=self.select_all,
                          var=self.selected_all)
        all.grid(column=0, row=0)

        self.canvas_frame = self.canvas.create_window((0,0),window=self.sensorsStatsFrame,anchor='nw')
        self.sensorsStatsFrame.bind("<Configure>", self.OnFrameConfigure)
        self.canvas.bind('<Configure>', self.FrameWidth)


    def select_all(self):
        if self.selected_all.get() == 1:
            for b in self.checkbuttons:
                b.select()
        else:
            for b in self.checkbuttons:
                b.deselect()

    def get_selected(self):
        ans = [ind for ind, elem in enumerate(self.elem_bools) if elem.get() == 1]
        return ans


MAIN_FRAME_WIDTH = 1200
MAIN_FRAME_HEIGHT = 800
TITLE = 'CFG Learner'
MLA_PATH = 'output_mla_manual2.txt'
FILE_PATH = 'michalTrees'
WEIGHTED = True
TREES_LIST = None


def get_trees_list(file_path, weighted=False):
    if weighted:
        trees_list, labels_dict, reverse_dict = load_trees_from_file(file_path)
        return trees_list, reverse_dict
    file = open(file_path)
    trees_list = json.load(file)
    d = trees_list['cogs_dict']['reverse_dict']
    d = {int(key): d[key] for key in d}
    trees_list = [(Tree.fromstring(tup[0]), tup[1]) for tup in trees_list['trees']]
    return trees_list, d


def onFrameConfigure(event, canvas):
    '''Reset the scroll region to encompass the inner frame'''
    canvas.configure(scrollregion=canvas.bbox("all"))


def learn_cmd(indices, tree_list, d):
    trees = [tree_list[ind][0] for ind in indices]
    teacher = SimpleTeacher()
    for tree in trees:
        teacher.addPositiveExample(tree)
    cfg = learn(teacher, d)
    print(cfg)


def learn_cmd_prob(indices, tree_list, reverse_dict):
    trees = [tree_list[ind] for ind in indices]
    teacher = SimpleMultiplicityTeacher(epsilon=0.0005, default_val=0)
    for tree in trees:
        teacher.addExample(*tree)

    def thread_task():
        print('starting')
        t = time()
        acc = learnMultPos(teacher)
        g = convert_pmta_to_pcfg(acc, reverse_dict)
        print(g)
        print('took {}'.format(time()-t))
    thread = Thread(target=thread_task)
    thread.start()


def init_GUI(width, height, title=TITLE):
    top = tkinter.Tk()
    top.geometry('{0}x{1}'.format(width, height))
    top.title(title)
    text = Label(top, width=20)
    myFont = Font(family="Arial", size=16)
    text.configure(font=myFont)
    text.grid(column=0, row=1)
    return top


def convert_tree(tree, d):
    for ind in tree.treepositions():
        t = tree[ind]
        if len(t) == 0:
            t.set_label(d[int(tree[ind].label())])


def draw_tree(tree, cf, text_box, d):
    cf.canvas().delete('all')
    tc = TreeWidget(cf.canvas(), tree, xspace=50, yspace=50,
                    line_width=2, node_font=('helvetica', -28))

    def show_description(sth):
        t = sth.__repr__()
        t = t.replace('\'', '')
        t = int(t[t.find('Text:')+len('Text: '):t.find(']')])
        text_box['text'] = d[t]
    tc.bind_click_nodes(show_description, button=1)
    cf.add_widget(tc, 400, 0)


def onselect(evt, trees_list, cf, text_box, d, weight_text=None):
    w = evt.widget
    index = int(w.curselection()[0])
    tree = trees_list[index][0]
    weight_text['text'] = 'Probability:{0:.4f}'.format(trees_list[index][1])
    draw_tree(tree, cf, text_box, d)


def add_trees_list(top, trees_list, d, weighted=False):
    listbox = Listbox(top, selectmode=tkinter.SINGLE)
    listbox.grid(column=0, row=0)
    for ind, item in enumerate(trees_list):
        listbox.insert(ind, 'tree{}'.format(ind+1))
    out_frame = Frame(top)
    out_frame.grid(column=0, row=2)
    c = Checklist(range(len(trees_list)), out_frame)
    c.pack()
    cmd = lambda: learn_cmd(c.get_selected(), trees_list, d)
    if weighted:
        cmd = lambda: learn_cmd_prob(c.get_selected(), trees_list, d)
    b = Button(out_frame, pady=10, text='OK', command=cmd)
    b.pack()
    tree_frame = Frame(top)
    tree_frame.grid(column=1, row=0, rowspan=2)
    cf = CanvasFrame(tree_frame)
    cf.canvas()['width'] = 1000
    cf.pack()
    myFont = Font(family="Arial", size=16)
    text = Label(top, width=20)
    text.configure(font=myFont)
    text.grid(column=0, row=1)
    weight_text = None
    if weighted:
        weight_text = Label(top, width=20)
        weight_text.configure(font=myFont)
        weight_text.grid(column=1, row=1)
    listbox.bind('<<ListboxSelect>>', lambda e: onselect(e, trees_list, cf, text, d,
                                                         weight_text=weight_text))


def create_window(root):
    window = Toplevel(root)
    window.geometry('{0}x{1}'.format(400, 400))
    b = Button(window, text='close', command=lambda: print('hello'))
    b.grid(column=0, row=0)


def convert_string(curr_str, alphabet, alphabet_rev, last_ind):
    for ind, elem in enumerate(curr_str):
        if elem not in alphabet:
            alphabet[elem] = last_ind[0]
            alphabet_rev[last_ind[0]] = elem
            last_ind[0] += 1
        curr_str[ind] = alphabet[elem]


def get_score_table(sequences):
    table = {}
    for seq, occ in sequences:
        for i in range(0, len(seq)-2):
            subseq = seq[i: i+2]
            if subseq not in table:
                table[subseq] = occ
            else:
                table[subseq] += occ
    return table


def pre_process_seq(seq):
    pairs = []
    ans = []
    last_ind = 0
    for i in range(len(seq)):
        if seq[last_ind] != seq[i]:
            pairs.append((last_ind, i-1))
            last_ind = i
    pairs.append((last_ind, len(seq)-1))
    pairs = list(filter(lambda p: p[0] != p[1], pairs))
    last = 0
    for i1, i2 in pairs:
        ans = ans + seq[last:i1] + ['{}${}'.format(seq[i1], (i2-i1+1))]
        last = i2+1
    ans = ans + seq[last:]
    return ans


def pre_process_tree(tree, rev_map, alphabet):
    for ind in tree.treepositions():
        t = tree[ind]
        if len(t) == 0:
            lbl = rev_map[int(tree[ind].label())]
            if '$' in lbl:
                lbl = lbl.split('$')
                lbl, occ = lbl
                occ = int(occ)
                tree[ind] = Tree(0, [Tree(alphabet[lbl], [])]*occ)
    return tree


def read_strings(file_path):
    seq_dict = {}
    alphabet = {}
    alphabet_rev = {}
    curr_ind = [0]
    with open(file_path) as file:
        data = json.load(file)
        for seq, occ in data:
            seq = pre_process_seq(seq)
            convert_string(seq, alphabet, alphabet_rev, curr_ind)
            s = tuple(seq)
            if s not in seq_dict:
                seq_dict[s] = occ
            else:
                seq_dict[s] += occ
    sequences = []
    for seq in seq_dict.keys():
        sequences.append((seq, seq_dict[seq]))
    table = get_score_table(sequences)
    return sequences, alphabet, alphabet_rev, table


def convert_tree_to_cnf(tree):
    treetransforms.chomsky_normal_form(tree)
    for ind in tree.treepositions():
        t = tree[ind]
        if len(t) != 0:
            tree[ind].set_label('0')


def normalize_trees(trees):
    total_sum = sum([occ for _, occ in trees])
    for ind, tup in enumerate(trees):
        trees[ind] = (tup[0], tup[1]/total_sum)


def create_trees(sequences, table, alphabet_rev, alphabet, contract=False):
    ans = []
    constructor = TreeConstructor(table)
    constructor.set_contract(contract)
    for seq, occ in sequences:
        curr_tree = constructor.construct_tree(seq)
        if contract:
            convert_tree_to_cnf(curr_tree)
        ans.append((curr_tree, occ))
    normalize_trees(ans)
    for ind, tree in enumerate(ans):
        ans[ind] = (pre_process_tree(tree[0], alphabet_rev, alphabet), tree[1])
    return ans


if __name__ == '__main__':
    args = sys.argv
    if len(args) < 2:
        print('Not enough arguments')
        exit()
    sequences, alphabet, alphabet_rev, table = read_strings(args[1])
    trees = create_trees(sequences, table, alphabet_rev, alphabet)
    top = init_GUI(MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT)
    trees_list, d = get_trees_list(FILE_PATH, weighted=WEIGHTED)
    add_trees_list(top, trees, alphabet_rev, weighted=WEIGHTED)
    b = Button(top, text="Create new window", command=lambda: create_window(top))
    b.grid(column=0, row=4)
    top.mainloop()
