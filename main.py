import json
import sys
import tkinter
from tkinter import *
from tkinter.font import Font
from time import time
from nltk import Tree
from itertools import product
from nltk import PCFG
from nltk.parse.viterbi import ViterbiParser
from nltk import treetransforms
from nltk.draw.util import CanvasFrame
from nltk.draw import TreeWidget
from CFGLearner import TreeComparator, SimpleMultiplicityTeacher, learnMultPos, set_verbose,  \
    DifferenceMultiplicityTeacher, LoggingLevel, TreeConstructor, LOG_DETAILS, LOG_DEBUG, DuplicationComparator, \
    SwapComparator, ProbabilityTeacher
from WCFG import convert_pmta_to_pcfg, compress_grammar, create_duplications
from threading import Thread
from GUI import Table, MapTable, Checklist, PopupWindow, load_object_window, HidableFrame
from PIL import Image, ImageFont, ImageDraw
from TreesConverter import TreesConverter
from PyPDF2 import PdfFileReader, PdfFileWriter
import os

MAIN_FRAME_WIDTH = 1200
MAIN_FRAME_HEIGHT = 1000
TITLE = 'PCFG Learner'


def make_tree_nodes_int(tree):
    new_label = '?' if tree.label() == '0' else int(tree.label())
    return Tree(new_label, [make_tree_nodes_int(child) for child in tree])


def get_nonterminals(pcfg):
    prods = pcfg.productions()
    alphabet = set()
    for prod in prods:
        for r in prod.rhs():
            if type(r) is str:
                alphabet.add(r)
    return list(alphabet)


def fix_tree(tree):
    if type(tree) is not Tree:
        return Tree(tree, [])
    return Tree(tree.label(), [fix_tree(child) for child in tree])


def merge_pdfs(input, output):
    if len(input) == 1:
        return
    input_streams = []
    with open(output, 'w+b') as output_stream:
        try:
            for input_file in input:
                input_streams.append(open(input_file, 'rb'))
            writer = PdfFileWriter()
            for reader in map(PdfFileReader, input_streams):
                writer.addPage(reader.getPage(0))
            writer.write(output_stream)
        finally:
            for f in input_streams:
                f.close()
            for name in input:
                os.remove(name)


def get_parse_options(seq, alphabet):
    unknown_indices = [ind for ind, s in enumerate(seq) if s == UNKNOWN]
    if len(unknown_indices) > 0:
        #TODO : CHANGE THAT
        for tup in product(*[alphabet]*1):
            curr_seq = [s for s in seq]
            for ind in unknown_indices:
                curr_seq[ind] = tup[0]
            yield curr_seq
    else:
        yield seq


class MainGUI:

    def __init__(self, title, width, height):
        self._top = None
        self._tree_frame = None
        self._secondary_tree_frame = None
        self._title = title
        self._width = width
        self._height = height
        self._table = None
        self._parse_button = None
        self._pcfg = None
        self._parsed_trees = []
        self._tree_converter = TreesConverter()
        self._tree_list = None
        self._oracle_settings = {'type': SimpleMultiplicityTeacher, 'args': [0.0005, 0], 'additional_settings': [],
                                 'equiv_settings': (4, 5000), 'comparator': None}

    def set_pcfg(self, pcfg):
        self._pcfg = pcfg
        self._parse_button['state'] = 'normal'

    def parse_command(self, seqs, keep=3):
        non_terminals = get_nonterminals(self._pcfg)
        viterbi = ViterbiParser(self._pcfg)
        for seq, id in seqs:
            curr_trees = []
            for parse_option in get_parse_options(seq, non_terminals):
                try:
                    for t in viterbi.parse(parse_option):
                        curr_trees.append((t, parse_option))
                except ValueError:
                    print(parse_option)
            print(curr_trees)
            curr_trees = sorted(curr_trees, key=lambda tree: -tree[0].prob())
            print(seq, sum([tree[0].prob() for tree in curr_trees]), len(curr_trees))
            if keep != -1:
                curr_trees = curr_trees[:keep]
            print('now', len(curr_trees))
            for tree, parse_option in curr_trees:
                self._parsed_trees.append((parse_option, tree, id))
        print(len(seqs), len(self._parsed_trees))
        trees = [(tree[0], tree[1], tree[2]) for tree in self._parsed_trees]
        output_files = []
        for i, (option, tree, ind) in enumerate(trees):
            a = save_tree(tree, None, 'parse{}'.format(i), postscript=False, prob=tree.prob(), csb_id=ind)
            output_files.append(a)
        merge_pdfs(output_files, 'merged_parse.pdf')

    def load_grammar(self):
        grammar = load_object_window()
        print(grammar)
        grammar = PCFG.fromstring(grammar)
        self.set_pcfg(grammar)

    def add_text_box(self, frame, name, row, lbl_kwargs={}, txt_kwargs={}):
        if 'text' not in lbl_kwargs:
            lbl_kwargs['text'] = name
        lbl = Label(frame, **lbl_kwargs)
        txt = Entry(frame, **txt_kwargs)
        lbl.grid(row=row, column=0)
        txt.grid(row=row, column=1)

        return lbl, txt


    @staticmethod
    def check_is_int(n):
        try:
            n = int(n)
        except Exception:
            return False
        return n > 0

    @staticmethod
    def check_is_prob(p):
        try:
            p = float(p)
        except Exception:
            return False
        return 0 <= p <= 1

    def save_trees(self, lst, button):
        top = self._top
        w = PopupWindow(top, 'Choose file name')
        button["state"] = "disabled"
        top.wait_window(w.top)
        button["state"] = "normal"
        if w.value is not None:
            trees = [(str(tree), prob) for tree, prob in lst.get_all_elements()]
            with open('{}.json'.format(w.value), 'w') as json_file:
                json.dump(trees, json_file)

    def load_trees(self, lst):
        top = self._top
        trees_list = load_object_window()
        trees_list = [(Tree.fromstring(str_repr), prob) for str_repr, prob in trees_list]
        lst.remove_all_elements()
        for tree, prob in trees_list:
            lst.add_elem('tree', (tree, prob))

    def add_trees_list(self, frame, row_ind=0, col_ind=0):

        def show_tree(tree, prob, cf):
            cf.canvas().delete('all')
            tc = TreeWidget(cf.canvas(), tree, xspace=50, yspace=50,
                            line_width=2, node_font=('helvetica', -28))
            cf.add_widget(tc, 400, 0)
            tp = TreeWidget(cf.canvas(), 'p={}'.format(prob))
            cf.add_widget(tp, 200, 200)

        canvasFrameSuper = Frame(frame)
        canvasFrameSuper.grid(row=1, column=1)
        cf = CanvasFrame(canvasFrameSuper)
        cf.canvas()['width'] = 1000
        cf.canvas()['height'] = 400
        cf.pack()
        tree_frame = Frame(frame)
        tree_frame.grid(column=col_ind, row=row_ind, rowspan=2)
        lst = Checklist([], [], tree_frame, command=lambda a: show_tree(*lst.elements[a], cf))
        lst.grid(row=1, column=0, rowspan=1, columnspan=2)
        save_trees_button = Button(tree_frame, text='Save trees', command=lambda: self.save_trees(lst,
                                                                                                  save_trees_button))
        save_trees_button.grid(row=2, column=0)
        load_trees_button = Button(tree_frame, text='Load trees', command=lambda: self.load_trees(lst))
        load_trees_button.grid(row=2, column=1)
        return lst

    def construct_tree_from_string(self, string):
        table = self._table.rows
        table = {self._tree_converter.convert_ngram(key): val for key, val in table}
        con = TreeConstructor(table)
        con.set_concat(True)
        con.set_lambda(1.0)
        ngram = self._tree_converter.convert_ngram(string.split(' '))
        tree = make_tree_nodes_int(con.construct_tree(ngram))
        return self._tree_converter.reverse_convert_tree(tree)

    def init_tree_adder_frame(self, lst, row=0, column=0, column_span=3):

        def add_tree(tree, str_to_tree, prob, lst):
            if str_to_tree != '' and tree != '':
                tkinter.messagebox.showerror("Add tree", "Can't enter both a tree and a string to tree")
                return
            if not MainGUI.check_is_prob(prob):
                tkinter.messagebox.showerror("Add tree", "Invalid probability")
                return
            prob = float(prob)
            t = None
            if str_to_tree != '':
                lst.add_elem('tree', (self.construct_tree_from_string(str_to_tree), prob))
            else:
                try:
                    t = Tree.fromstring(tree)
                    t = fix_tree(t)
                    self._tree_converter.convert_tree(t)
                except Exception:
                    tkinter.messagebox.showerror("Add tree", "Not a valid tree")
                    return

                lst.add_elem('tree', (t, prob))

        def load_sequences(lst):
            seq_list = load_object_window()
            for seq, prob in seq_list:
                add_tree('', seq, prob, lst)

        secondaryFrame = Frame(self._top)
        secondaryFrame.grid(row=row, column=column, columnspan=column_span)
        addTreesFrame = Frame(secondaryFrame)
        addTreesFrame.grid(row=0, column=0, columnspan=2)
        _, tree_txt = self.add_text_box(addTreesFrame, 'enter_tree', 0, lbl_kwargs={'text': 'Tree'},
                                                                        txt_kwargs={'width': 50})
        _, str_txt = self.add_text_box(addTreesFrame, 'enter_string', 1, lbl_kwargs={'text': 'String'},
                                        txt_kwargs={'width': 50})
        _, prob_txt = self.add_text_box(addTreesFrame, 'enter_prob', 2, lbl_kwargs={'text': 'Prob'},
                                        txt_kwargs={'width': 10})
        addTreeButton = Button(addTreesFrame, text='Add tree', command=lambda: add_tree(tree_txt.get(),
                                                                                        str_txt.get(),
                                                                                        prob_txt.get(),
                                                                                        lst))
        loadSequencesButton = Button(addTreesFrame, text='Load Sequences', command=lambda: load_sequences(lst))
        addTreeButton.grid(row=3, column=1)
        loadSequencesButton.grid(row=3, column=2)
        self._secondary_tree_frame = secondaryFrame
        self._tree_list = lst

    def learn_cmd_prob(self):
        lst = self._tree_list
        oracle_settings = self._oracle_settings

        def thread_task():
            table = self._table.rows
            table = {self._tree_converter.convert_ngram(key): val for key, val in table}
            if oracle_settings['type'] is ProbabilityTeacher:
                print(oracle_settings)
                d = oracle_settings['comparator']()
                teacher = ProbabilityTeacher(d, oracle_settings['args'][0], 0.000001)
            else:
                teacher = oracle_settings['type'](*oracle_settings['args'])
            for tree, prob in lst.get_selected_elements():
                teacher.addExample(self._tree_converter.convert_tree(tree), prob)

            if oracle_settings['comparator'] is SwapComparator:
                con = TreeConstructor(table)
                con.set_concat(True)
                con.set_lambda(1.0)
                teacher.setup_constructor_generator(con, *oracle_settings['equiv_settings'])
            if oracle_settings['comparator'] is DuplicationComparator:
                teacher.setup_duplications_generator(2)
            #set_verbose(LOG_DEBUG)
            print('starting')
            t = time()
            acc = learnMultPos(teacher)
            g = convert_pmta_to_pcfg(acc, self._tree_converter.reverse_dict)
            g = convert_pcfg_to_str(g)
            with open('grammar.json', 'w') as json_file:
                json.dump(g, json_file)
            with open('grammar.txt', 'w') as output:
                output.write(g)
            print(g)
        thread = Thread(target=thread_task)
        thread.start()

    def create_scores_table(self, row=1, column=0):
        t = Table([], ['Seq', 'Score'], [False]*2, self._top)
        t.grid(row=row, column=column)
        self._table = t

    def init_GUI(self):
        self._top = tkinter.Tk()
        self._top.geometry('{0}x{1}'.format(self._width, self._height))
        self._top.title(self._title)
        lst = self.add_trees_list(self._top)
        self.init_tree_adder_frame(lst)
        self.create_scores_table(row=3)
        self.create_oracle_frame(first_row=0)
        learn_button = Button(self._top, text='Learn', command=lambda: self.learn_cmd_prob())
        learn_button.grid(row=2, column=1)

    def main_loop(self):
        self._top.mainloop()

    def update_oracle(self, prob_dup, prob_swap, max_len, num_examples):
        if prob_dup is None and prob_swap is None:
            self._oracle_settings['type'] = SimpleMultiplicityTeacher
            self._oracle_settings['comparator'] = None
            return
        if not self.check_is_int(max_len):
            tkinter.messagebox.showerror("Update Oracle", "Invalid max length")
            return
        max_len = int(max_len)
        if num_examples != -1 and not self.check_is_int(num_examples):
            tkinter.messagebox.showerror("Update Oracle", "Invalid num of examples")
            return
        num_examples = int(num_examples)
        if prob_dup is not None and not self.check_is_prob(prob_dup):
            tkinter.messagebox.showerror("Update Oracle", "Invalid duplication probability")
            return
        if prob_swap is not None and not self.check_is_prob(prob_swap):
            tkinter.messagebox.showerror("Update Oracle", "Invalid swap probability")
            return
        if prob_dup == 1:
            tkinter.messagebox.showerror("Update Oracle", "Duplication probability can't be 1")
            return

        def add_constructor_generator(teacher, **kwargs):
            if 'table' not in kwargs:
                raise BaseException("no table")
            table = kwargs['table']
            con = TreeConstructor(table)
            con.set_concat(True)
            con.set_lambda(1.0)
            teacher.setup_constructor_generator(con, max_len, num_examples)

        def add_duplications_generator(teacher, **kwargs):
            teacher.setup_duplications_generator(2)

        self._oracle_settings['type'] = ProbabilityTeacher
        self._oracle_settings['equiv_settings'] = (max_len, num_examples)

        if prob_swap is not None:
            prob_swap = float(prob_swap)

            self._oracle_settings['comparator'] = SwapComparator
            self._oracle_settings['args'] = [prob_swap]
            self._oracle_settings['additional_settings'] = [lambda t, **kwargs: add_constructor_generator(t, **kwargs)]

        if prob_dup is not None:
            prob_dup = float(prob_dup)
            self._oracle_settings['comparator'] = DuplicationComparator
            self._oracle_settings['args'] = [prob_dup]
            self._oracle_settings['additional_settings'] = [lambda t, **kwargs: add_duplications_generator(t, **kwargs)]

    def create_oracle_frame(self, first_row=0):
        #oracle_frame = Frame(self._secondary_tree_frame)
        oracle_frame = Frame(self._top)
        oracle_frame.grid(column=0, row=first_row+2)
        edit_dist_label = Label(oracle_frame, text='Edit distance options')
        edit_dist_label.grid(row=first_row, column=0)
        duplications_frame = HidableFrame("Allow duplications", oracle_frame)
        duplications_frame.grid(row=first_row+1, column=0)
        duplications_frame.add_var('prob', desc='Duplication Probability')
        swap_frame = HidableFrame("Allow swaps", oracle_frame)
        swap_frame.grid(row=first_row+2, column=0)
        swap_frame.add_var('prob', desc='Swap Probability')
        equiv_title = Label(oracle_frame, text='Equivalence query settings')
        max_len_label = Label(oracle_frame, text='Max example length')
        max_len_entry = Entry(oracle_frame)
        random_sampling = HidableFrame("Random sampling", oracle_frame)
        random_sampling.add_var('num_samples', desc='Samples Number')
        random_sampling.grid(row=first_row+4, column=0)
        equiv_title.grid(row=first_row+3, column=0, columnspan=2)
        max_len_label.grid(row=first_row+5, column=0)
        max_len_entry.grid(row=first_row+5, column=1)
        max_len_entry.insert(END, self._oracle_settings['equiv_settings'][0])

        def set_oracle():
            dup_vis = duplications_frame.get_visible()
            swap_vis = swap_frame.get_visible()
            num_examples = -1 if not random_sampling.get_visible() else random_sampling.get_val('num_samples')
            dup_prob = None if not dup_vis else duplications_frame.get_val('prob')
            swap_prob = None if not swap_vis else swap_frame.get_val('prob')
            self.update_oracle(dup_prob, swap_prob, max_len_entry.get(), num_examples)

        set_oracle_button = Button(oracle_frame, text="Set", command=set_oracle)
        set_oracle_button.grid(row=first_row+6, column=0)

    def save_trees_popup(self, trees, d, button):
        top = self._top
        seq = self.sequences
        w = PopupWindow(top, 'Choose file name')
        button["state"] = "disabled"
        top.wait_window(w.top)
        button["state"] = "normal"
        file_names = []
        if w.value is not None:
            if len(trees) == 1:
                save_tree(trees[0], d, w.value)
            else:
                for ind, tree in enumerate(trees):
                    file_names.append(save_tree(tree, d, '{}{}'.format(w.value, ind), csb_id=seq[ind]['csb_id'],
                                                instances=seq[ind]['instances'], p=tree[1]))
            merge_pdfs(file_names, '{}.pdf'.format(w.value))


def convert_pcfg_to_str(pcfg):
    lst = str(pcfg).split('\n')
    lst = lst[1:]
    for i, l in enumerate(lst):
        lst[i] = l.strip()
    ans = '\n'.join(lst)
    return ans


def get_nonterminals(pcfg):
    nt = set([prod.lhs() for prod in pcfg.productions()])
    return list(nt)


def get_prod_by_nt(pcfg, nt):
    ans = [prod for prod in pcfg.productions() if prod.lhs()==nt]
    ans = sorted(ans, key=lambda a: a.prob())
    return ans[::-1]


def learn_prob(trees, reverse_dict, oracle_settings, table):
    print(oracle_settings['equiv_settings'])
    if oracle_settings['type'] is ProbabilityTeacher:
        print(oracle_settings)
        d = oracle_settings['comparator']()
        teacher = ProbabilityTeacher(d, oracle_settings['args'][0], 0.001)
    else:
        teacher = oracle_settings['type'](*oracle_settings['args'])
    for tree in trees:
        teacher.addExample(*tree)
    if oracle_settings['type'] is ProbabilityTeacher:
        con = TreeConstructor(table)
        con.set_concat(True)
        con.set_lambda(1.0)
        teacher.setup_constructor_generator(con, *oracle_settings['equiv_settings'])
    print('setting')
    # set_verbose(LOG_DEBUG)
    print('starting')
    t = time()
    acc = learnMultPos(teacher)
    acc.print_desc()
    g = convert_pmta_to_pcfg(acc, reverse_dict)
    #gui.set_pcfg(g)
    with open('output_g', 'w') as output_file:
        for nt in get_nonterminals(g):
            output_file.write('\startprod{%s} ' % nt)
            prod_list = get_prod_by_nt(g, nt)
            if len(prod_list) <= 4:
                for prod in prod_list:
                    rhs_str = ' '.join([str(r) for  r in prod.rhs()])
                    output_file.write('\derivation{%s}{%.3f}{%d}' % (rhs_str, prod.prob(), int(100*prod.prob())))
            else:
                for i in range(0, len(prod_list), 4):
                    curr_sub_list = prod_list[i:min(i+4, len(prod_list))]
                    output_file.write('\makebox[10cm]{')
                    for prod in curr_sub_list:
                        rhs_str = ' '.join([str(r) for  r in prod.rhs()])
                        output_file.write('\derivation{%s}{%.3f}{%d}' % (rhs_str, prod.prob(),
                                                                         int(100*prod.prob())))
                    output_file.write('}\n\n')
            output_file.write('\n\n')
    g_str = convert_pcfg_to_str(g)
    with open('grammar.json', 'w') as json_file:
        json.dump(g_str, json_file)
    print('took {}'.format(time()-t))
    return g


def print_cfg(pcfg):
    print(pcfg)


def convert_tree(tree, d):
    for ind in tree.treepositions():
        t = tree[ind]
        if len(t) == 0:
            t.set_label(d[int(tree[ind].label())])


def remap_tree(tree, seq):
    positions = tree.treepositions()
    positions = [pos for pos in positions if not isinstance(tree[pos], Tree) or len(tree[pos]) == 0]
    assert(len(positions) == len(seq))
    for ind, token in zip(positions, seq):
        if isinstance(token, tuple):
            sub_tree = Tree(0, [Tree(t, []) for t in token])
            if ind == ():
                return sub_tree
            tree[ind] = sub_tree
        else:
            tree[ind].set_label(token)
    return tree


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


def onselect2(index, trees_list, cf, text_box, d, weight_text=None):
    tree = trees_list[index][0]
    prob = trees_list[index][1]
    weight_text['text'] = 'Probability:{0:.4f}'.format(trees_list[index][1])
    draw_tree(tree, cf, text_box, d, prob=prob)


def save_tree(tree, d, name, postscript=False, **kwargs):
    drawable_tree = tree if d is None else get_drawable_tree(tree[0], d)
    extension = 'ps' if postscript else 'png'
    cf = CanvasFrame()
    tc = TreeWidget(cf.canvas(), drawable_tree)
    tc['node_font'] = 'arial 22 bold'
    tc['leaf_font'] = 'arial 22'
    tc['node_color'] = '#005990'
    tc['leaf_color'] = '#3F8F57'
    tc['line_color'] = '#175252'
    tc['xspace'] = 20
    tc['yspace'] = 20
    curr_y = 40*len(kwargs)
    cf.add_widget(tc, 0, curr_y)
    cf.print_to_file('{0}.{1}'.format(name, extension))
    if not postscript:
        im1 = Image.open('{0}.{1}'.format(name, extension))
        im1 = im1.convert('RGB')
        curr_y = 10
        for key in kwargs:
            font = ImageFont.truetype("/fonts/Ubuntu-L.ttf", 24)
            draw = ImageDraw.Draw(im1)
            val = kwargs[key]
            format_str = '{0}'
            if isinstance(val, float):
                format_str = format_str+'={1:.4f}'
            else:
                format_str = format_str+'=  {1}'
            draw.text((10, curr_y), format_str.format(key, val), (0, 0, 0), font)
            curr_y = curr_y + 40
        im1.save('{}.pdf'.format(name))
        cf.destroy()
        os.remove('{0}.{1}'.format(name, extension))
    return '{0}.{1}'.format(name, 'ps' if postscript else 'pdf')


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


def get_all_score_tuples(seq, alphabet, alphabet_rev):
    yield seq


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


def create_seq(seq, indices):
    if len(indices) == 0:
        return seq
    ans = []
    ind = 0
    while ind < len(seq):
        if len(indices) == 0:
            ans = ans + list(seq[ind:])
            break
        if ind == indices[0][0]:
            tup = indices[0]
            ans.append(tuple(seq[tup[0]: tup[1]]))
            indices = indices[1:]
            ind = tup[1]
        else:
            ans.append(seq[ind])
            ind = ind + 1
    return ans


def create_trees(sequences, table, contract=False, lambda_val=0.0, key='annot'):
    ans = []
    seqs = []
    constructor = TreeConstructor(table)
    constructor.set_lambda(lambda_val)
    constructor.set_concat(True)
    sequences = filter(lambda a: a is not None, sequences)
    for row in sequences:
        curr_tree = constructor.construct_tree(row[key])
        if contract:
            convert_tree_to_cnf(curr_tree)
        ans.append((curr_tree, row['instances']))
    normalize_trees(ans)
    print(ans)
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


def create_swapped_tree(tree):
    return Tree(0, [tree[1], tree[0]])


def get_yield(tree, alphabet_rev):
    ans = []
    for ind in tree.treepositions():
        t = tree[ind]
        if len(t) == 0:
            ans.append(alphabet_rev[int(t.label())])
    return ans


def get_table_tests():
    return [(('a', 'b'), 5), (('b', 'a'), 0)]


def get_trees_tests():
    tree1 = Tree('?', [Tree('a', []), Tree('b', [])])
    tree2 = Tree('?', [Tree('a', []), tree1])
    return [(tree1, 0.6), (tree2, 0.4)]


def simple_learning_test():
    converter = TreesConverter()
    table = get_table_tests()
    table = {converter.convert_ngram(key): val for key, val in table}
    trees = get_trees_tests()
    con = TreeConstructor(table)
    con.set_concat(True)
    con.set_lambda(1.0)
    teacher = SimpleMultiplicityTeacher(epsilon=0.000001, default_val=0)
    for tree, prob in trees:
        teacher.addExample(converter.convert_tree(tree), prob)
    print('starting')
    t = time()
    acc = learnMultPos(teacher)
    g = convert_pmta_to_pcfg(acc, converter.reverse_dict)
    g = convert_pcfg_to_str(g)
    with open('grammar_simple.txt', 'w') as file:
        file.write(g)
    print(g)


def swap_learning_test():
    converter = TreesConverter()
    table = get_table_tests()
    table = {converter.convert_ngram(key): val for key, val in table}
    trees = get_trees_tests()
    con = TreeConstructor(table)
    con.set_concat(True)
    con.set_lambda(1.0)
    cmp = SwapComparator()
    teacher = ProbabilityTeacher(cmp, 0.25, 0.0000001)
    for tree, prob in trees:
        teacher.addExample(converter.convert_tree(tree), prob)
    teacher.setup_constructor_generator(con, 4, -1)
    print('starting')
    t = time()
    acc = learnMultPos(teacher)
    g = convert_pmta_to_pcfg(acc, converter.reverse_dict)
    g = convert_pcfg_to_str(g)
    with open('grammar_swap.txt', 'w') as file:
        file.write(g)
    print(g)


def dup_learning_test():
    converter = TreesConverter()
    trees = get_trees_tests()
    cmp = DuplicationComparator()
    teacher = ProbabilityTeacher(cmp, 0.2, 0.0000001)
    for tree, prob in trees:
        teacher.addExample(converter.convert_tree(tree), prob)
    teacher.setup_duplications_generator(2)
    print('starting')
    t = time()
    acc = learnMultPos(teacher)
    g = convert_pmta_to_pcfg(acc, converter.reverse_dict)
    g = convert_pcfg_to_str(g)
    with open('grammar_dup.txt', 'w') as file:
        file.write(g)
    print(g)



def auto_tests(args):
    simple_learning_test()
    swap_learning_test()
    dup_learning_test()

"""
    if oracle_settings['type'] is ProbabilityTeacher:
        print(oracle_settings)
        d = oracle_settings['comparator']()
        teacher = ProbabilityTeacher(d, oracle_settings['args'][0], 0.001)
    else:
        teacher = oracle_settings['type'](*oracle_settings['args'])
"""
"""
    if oracle_settings['comparator'] is SwapComparator:
        con = TreeConstructor(table)
        con.set_concat(True)
        con.set_lambda(1.0)
        teacher.setup_constructor_generator(con, *oracle_settings['equiv_settings'])
    if oracle_settings['comparator'] is DuplicationComparator:
        teacher.setup_duplications_generator(2)
"""


if __name__ == '__main__':
    args = sys.argv
    if args[-1] == '-test':
        auto_tests(args)
        exit()
    gui = MainGUI(TITLE, MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT)
    gui.init_GUI()
    gui.main_loop()
