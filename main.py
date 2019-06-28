import tkinter
from tkinter import Listbox, Text, INSERT, END
from tkinter.font import Font
import json
import random
from nltk import Tree
from nltk.draw.util import CanvasFrame
from nltk.draw import TreeWidget


MAIN_FRAME_WIDTH = 1200
MAIN_FRAME_HEIGHT = 800
TITLE = 'CFG Learner'
mla = open('output_mla_manual2.txt')
mla_list = json.load(mla)
d = mla_list['cogs_dict']['reverse_dict']
d = {int(key): d[key] for key in d}
mla_list = [(Tree.fromstring(tup[0]), tup[1]) for tup in mla_list['trees']]

top = tkinter.Tk()
cf = CanvasFrame(top)
cf.canvas()['width'] = 1000
cf.pack(side='right')
text = Text(top)
myFont = Font(family="Arial", size=16)
text.configure(font=myFont)
text.pack(side='bottom')
top.geometry('{0}x{1}'.format(MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT))
top.title(TITLE)
listbox = Listbox(top, selectmode=tkinter.SINGLE)
listbox.pack()

for ind, item in enumerate(mla_list):
    listbox.insert(ind, 'tree{}'.format(ind+1))


def draw_tree(tree, cf):
    cf.canvas().delete('all')
    tc = TreeWidget(cf.canvas(), tree, xspace=50, yspace=50,
                    line_width=2, node_font=('helvetica', -28))

    def print_something(sth):
        t = sth.__repr__()
        t = int(t[t.find('\'')+1:t.find(']')-1])
        text.delete(1.0, END)
        text.insert(INSERT, d[t])
    tc.bind_click_nodes(print_something, button=1)
    cf.add_widget(tc, 200, 0)


def onselect(evt):
    w = evt.widget
    index = int(w.curselection()[0])
    tree = mla_list[index][0]
    draw_tree(tree, cf)


listbox.bind('<<ListboxSelect>>', onselect)
top.mainloop()
