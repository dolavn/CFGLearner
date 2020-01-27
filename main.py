import tkinter
from tkinter import *
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
import itertools


class PopupWindow(object):
    def __init__(self, master, title=''):
        top = self.top = Toplevel(master)
        self.l = Label(top, text=title)
        self.l.grid(row=0, columnspan=2)
        self.e = Entry(top)
        self.e.grid(row=1, columnspan=2)
        self.b_commit = Button(top, text='Save', command=self.commit)
        self.b_cancel = Button(top, text='Cancel', command=self.cleanup)
        self.b_commit.grid(row=2, column=0)
        self.b_cancel.grid(row=2, column=1)


    def commit(self):
        self.value = self.e.get()
        self.top.destroy()

    def cleanup(self):
        self.value = None
        self.top.destroy()


class Table(Frame):

    def FrameWidth(self, event):
        canvas_width = event.width
        self.canvas.itemconfig(self.canvas_frame, width = canvas_width)

    def OnFrameConfigure(self, event):
        self.canvas.config(scrollregion=self.canvas.bbox("all"))

    def update_command_wrapper(self):
        for row_ind, row in enumerate(self._rows):
            mutable_row = list(row)
            for col_ind, col in enumerate(row):
                if col_ind in self.entries:
                    mutable_row[col_ind] = self.entries[col_ind][row_ind].get()
            self._rows[row_ind] = tuple(mutable_row)
        self._update_cmd(self._rows)

    def __init__(self, rows, cols, mutable_cols, *args, **kwargs):
        self._update_cmd = None
        if 'update_cmd' in kwargs:
            self._update_cmd = kwargs['update_cmd']
            del kwargs['update_cmd']  # Must delete since superclass constructor would throw an exception otherwise
        Frame.__init__(self, *args, **kwargs)
        self.rows_num = len(rows)
        self.cols_num = len(cols)
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self._rows = rows
        self._mutable_cols = mutable_cols
        sensorsFrame = Frame(self)
        sensorsFrame.grid(row=0, sticky="nsew")
        sensorsFrame.grid_rowconfigure(0, weight=1)
        sensorsFrame.grid_columnconfigure(0, weight=1)

        self.canvas = Canvas(sensorsFrame)
        self.sensorsStatsFrame = Frame(self.canvas)
        self.canvas.grid_columnconfigure(0, weight=1)
        self.sensorsStatsFrame.grid_columnconfigure(0, weight=1)

        verticalscrollbar = Scrollbar(sensorsFrame, orient=VERTICAL, command=self.canvas.yview)
        self.canvas.configure(yscrollcommand=verticalscrollbar.set)
        self.canvas.grid(column=0, sticky="nsew")
        verticalscrollbar.grid(row=0, column=1, sticky="nsew")
        horizontalscrollbar = Scrollbar(sensorsFrame, orient=HORIZONTAL, command=self.canvas.xview)
        self.canvas.configure(xscrollcommand=horizontalscrollbar.set)
        horizontalscrollbar.grid(row=1, column=0, sticky="nsew")
        for ind, col in enumerate(cols):
            header_label = Label(self.sensorsStatsFrame, text=col,
                                 font='Helvetica 18 bold')
            header_label.grid(row=0, column=ind)
        self.entries = {}
        for col in range(self.cols_num):
            if self._mutable_cols[col]:
                self.entries[col] = []
        for row, col in itertools.product(range(self.rows_num), range(self.cols_num)):
            if len(rows[row]) != self.cols_num:
                raise BaseException("Columns number in row {} don't match".format(row))
            cell_label = None
            if self._mutable_cols[col]:
                cell_label = Entry(self.sensorsStatsFrame)
                cell_label.insert(END, rows[row][col])
                self.entries[col].append(cell_label)
            else:
                cell_label = Label(self.sensorsStatsFrame, text=rows[row][col])
            cell_label.grid(row=row+1, column=col)
        if any(self._mutable_cols):
            change_button = Button(self.sensorsStatsFrame, text='Update', command=lambda: self.update_command_wrapper())
            change_button.grid(row=self.rows_num+1, columnspan=self.cols_num)
        self.canvas_frame = self.canvas.create_window((0, 0), window=self.sensorsStatsFrame, anchor='nw')
        self.sensorsStatsFrame.bind("<Configure>", self.OnFrameConfigure)


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
            b = Checkbutton(self.sensorsStatsFrame, text=str(elem),
                            var=self.elem_bools[ind])
            b.grid(column=0, row=ind+1)
            self.checkbuttons.append(b)


        all = Checkbutton(self.sensorsStatsFrame, text="Select all", command=self.select_all,
                          var=self.selected_all)
        all.grid(column=0, row=0)

        self.canvas_frame = self.canvas.create_window((0,0),window=self.sensorsStatsFrame, anchor='nw')
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
GUI_Attributes = {'Tree_Frame': None, 'Secondary_Tree_Frame': None,
                  'top': None}


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


def convert_tree(tree, d):
    for ind in tree.treepositions():
        t = tree[ind]
        if len(t) == 0:
            t.set_label(d[int(tree[ind].label())])


def copy_tree(tree):
    if len(tree) == 0:
        return Tree(tree.label(), [])
    ans = Tree(tree.label(), [copy_tree(child) for child in tree])
    return ans


def get_drawable_tree(tree, d):
    copy = copy_tree(tree)
    for ind in copy.treepositions():
        if len(copy[ind]) != 0:
            copy[ind].set_label('?')
            continue
        lbl = int(copy[ind].label())
        if lbl not in d:
            continue
        copy[ind].set_label(d[lbl])
    return copy


def draw_tree(tree, cf, text_box, d, prob=None):
    cf.canvas().delete('all')
    copy = get_drawable_tree(tree, d)
    tc = TreeWidget(cf.canvas(), copy, xspace=50, yspace=50,
                    line_width=2, node_font=('helvetica', -28))

    def show_description(sth):
        t = sth.__repr__()
        t = t.replace('\'', '')
        t = int(t[t.find('Text:')+len('Text: '):t.find(']')])
        text_box['text'] = d[t]
    tc.bind_click_nodes(show_description, button=1)
    cf.add_widget(tc, 400, 0)
    if prob is not None:
        tp = TreeWidget(cf.canvas(), prob)
        cf.add_widget(tp, 0, 400)


def onselect(evt, trees_list, cf, text_box, d, weight_text=None):
    w = evt.widget
    if len(w.curselection()) == 0:
        return
    index = int(w.curselection()[0])
    tree = trees_list[index][0]
    prob = trees_list[index][1]
    weight_text['text'] = 'Probability:{0:.4f}'.format(trees_list[index][1])
    draw_tree(tree, cf, text_box, d, prob=prob)


def save_tree(tree, d, name):
    drawable_tree = get_drawable_tree(tree[0], d)
    cf = CanvasFrame()
    tc = TreeWidget(cf.canvas(), drawable_tree)
    tc['node_font'] = 'arial 22 bold'
    tc['leaf_font'] = 'arial 22'
    tc['node_color'] = '#005990'
    tc['leaf_color'] = '#3F8F57'
    tc['line_color'] = '#175252'
    tc['xspace'] = 20
    tc['yspace'] = 20
    cf.add_widget(tc, 0, 0)
    cf.print_to_file('{0}.png'.format(name))
    cf.destroy()


def save_trees_popup(trees, d, button):
    top = GUI_Attributes['top']
    w = PopupWindow(top, 'Choose file name')
    button["state"] = "disabled"
    top.wait_window(w.top)
    button["state"] = "normal"
    if w.value is None:
        print('none')
    else:
        if len(trees) == 1:
            save_tree(trees[0], d, w.value)
        else:
            for ind, tree in enumerate(trees):
                save_tree(tree, d, '{}{}'.format(w.value, ind))


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
    for ngram_len in range(2, len(sequences)):
        for seq, occ in sequences:
            for i in range(0, len(seq)-ngram_len):
                subseq = seq[i: i+ngram_len]
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


def create_trees(sequences, table, alphabet_rev, alphabet, contract=False, lambda_val=0.0):
    ans = []
    constructor = TreeConstructor(table)
    for seq, occ in sequences:
        constructor.set_lambda(lambda_val)
        curr_tree = constructor.construct_tree(seq)
        if contract:
            convert_tree_to_cnf(curr_tree)
        ans.append((curr_tree, occ))
    normalize_trees(ans)
    for ind, tree in enumerate(ans):
        ans[ind] = (pre_process_tree(tree[0], alphabet_rev, alphabet), tree[1])
    return ans


def get_table_as_list(table, alphabet_rev):
    keys = table.keys()
    keys_copy = []
    for ind, key in enumerate(keys):
        keys_copy.append(tuple([alphabet_rev[k] for k in key]))
    return [(key, table[orig_key]) for key, orig_key in zip(keys_copy, keys)]


def update_table(table, alphabet, list):
    for key, val in list:
        key_new = tuple([alphabet[k] for k in key])
        table[key_new] = val
    print(table)


class MainGUI():

    def __init__(self, title, width, height):
        self._top = None
        self._tree_frame = None
        self._secondary_tree_frame = None
        self._title = title
        self._width = width
        self._height = height
        self._table = None

    def init_GUI(self, sequences, alphabet, alphabet_rev, table):
        self._top = tkinter.Tk()
        self._top.geometry('{0}x{1}'.format(self._width, self._height))
        self._top.title(self._title)
        self._table = table
        seqFrame = Frame(self._top)
        seqFrame.grid(row=0, column=0)
        treeFrame = Frame(self._top)
        treeFrame.grid(row=1, column=0)
        self.create_sequences_list(seqFrame, sequences, alphabet, alphabet_rev)
        self.add_trees_list(treeFrame, [], alphabet_rev, weighted=WEIGHTED)

    def main_loop(self):
        self._top.mainloop()

    def add_trees_list(self, top, trees_list, d, weighted=False):
        secondary_frame = Frame(top)
        secondary_frame.pack()
        listbox = Listbox(secondary_frame, selectmode=tkinter.SINGLE)
        listbox.grid(column=0, row=0)
        for ind, item in enumerate(trees_list):
            listbox.insert(ind, 'tree{}'.format(ind+1))
        out_frame = Frame(secondary_frame)
        out_frame.grid(column=0, row=2)
        c = Checklist(['Tree {}'.format(ind) for ind in range(len(trees_list))], out_frame)
        c.pack()
        cmd = lambda: learn_cmd(c.get_selected(), trees_list, d)
        if weighted:
            cmd = lambda: learn_cmd_prob(c.get_selected(), trees_list, d)
        learn_button = Button(secondary_frame, pady=10, text='Learn', command=cmd)
        learn_button.grid(column=0, row=3)
        save_frame = Frame(secondary_frame)
        save_frame.grid(column=1, row=3)
        save_tree_button = Button(save_frame, pady=10, text='Save tree',
                                  command=lambda: save_trees_popup([trees_list[0]], d, save_tree_button))
        save_tree_button.grid(column=0, row=0)
        save_all_trees_button = Button(save_frame, pady=10, text='Save all trees',
                                       command=lambda: save_trees_popup(trees_list, d, save_tree_button))
        save_all_trees_button.grid(column=1, row=0)
        tree_frame = Frame(secondary_frame)
        tree_frame.grid(column=1, row=0, rowspan=3)
        cf = CanvasFrame(tree_frame)
        cf.canvas()['width'] = 1000
        cf.canvas()['height'] = 400
        cf.pack()
        myFont = Font(family="Arial", size=16)
        text = Label(secondary_frame, width=20)
        text.configure(font=myFont)
        text.grid(column=0, row=1)
        weight_text = None
        if weighted:
            weight_text = Label(secondary_frame, width=20)
            weight_text.configure(font=myFont)
            weight_text.grid(column=1, row=4)
        listbox.bind('<<ListboxSelect>>', lambda e: onselect(e, trees_list, cf, text, d,
                                                             weight_text=weight_text))
        self._tree_frame = top
        self._secondary_tree_frame = secondary_frame

    def create_trees_command(self, lambda_val, sequences, alphabet, alphabet_rev):
        try:
            val = float(lambda_val)
        except Exception:
            tkinter.messagebox.showerror("Create Trees", "Invalid value of lambda")
            return
        table = self._table
        secondary_tree_frame = self._secondary_tree_frame
        tree_frame = self._tree_frame
        self._secondary_tree_frame = None
        secondary_tree_frame.destroy()
        trees = create_trees(sequences, table, alphabet_rev, alphabet, lambda_val=val)
        self.add_trees_list(tree_frame, trees, alphabet_rev, weighted=WEIGHTED)

    def create_sequences_list(self, top, sequences, alphabet_dict, alphabet_rev_dict):
        table = self._table
        out_frame = Frame(top)
        out_frame.grid(column=1, row=0)
        sequences_copy = sequences.copy()
        for ind, seq in enumerate(sequences):
            sequences_copy[ind] = (tuple([alphabet_rev_dict[elem] for elem in seq[0]]), seq[1])
        t = Table(sequences_copy, ['Sequence', 'Occurrences'], [False, False], out_frame)
        out_frame2 = Frame(top)
        out_frame2.grid(column=0, row=0)
        t2 = Table(get_table_as_list(table, alphabet_rev_dict), ['Subseq', 'Score'],
                   [False, True], out_frame2, update_cmd=lambda a: update_table(table, alphabet_dict, a))
        t2.pack()
        input_frame = Frame(top)
        input_frame.grid(column=2, row=0)
        lambda_label = Label(input_frame, text='Lambda val:')
        lambda_label.grid(column=0, row=0)
        lambda_text = Entry(input_frame, width=10)
        lambda_text.grid(column=1, row=0)
        create_trees_button = Button(input_frame, text='Create Trees',
                                     command=lambda: self.create_trees_command(lambda_text.get(),
                                                                               sequences, alphabet_dict,
                                                                               alphabet_rev_dict))
        create_trees_button.grid(column=0, row=1, columnspan=2)
        t.pack()


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


if __name__ == '__main__':
    args = sys.argv
    if len(args) < 2:
        print('Not enough arguments')
        exit()
    sequences, alphabet, alphabet_rev, table = read_strings(args[1])
    gui = MainGUI(TITLE, MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT)
    gui.init_GUI(sequences, alphabet, alphabet_rev, table)
    gui.main_loop()
