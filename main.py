import tkinter
from tkinter import Listbox, Frame, Canvas,\
    Scrollbar, Label, VERTICAL, Checkbutton, IntVar, Button
from tkinter.font import Font
import json
from nltk import Tree
from nltk.draw.util import CanvasFrame
from nltk.draw import TreeWidget
from CFGLearner import SimpleTeacher, FrequencyTeacher, DifferenceTeacher, Teacher, learn, TreeComparator


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
mla = open('output_mla_manual2.txt')
mla_list = json.load(mla)
d = mla_list['cogs_dict']['reverse_dict']
d = {int(key): d[key] for key in d}
mla_list = [(Tree.fromstring(tup[0]), tup[1]) for tup in mla_list['trees']]

top = tkinter.Tk()
top.geometry('{0}x{1}'.format(MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT))
top.title(TITLE)
text = Label(top, width=20)
myFont = Font(family="Arial", size=16)
text.configure(font=myFont)
text.grid(column=0, row=1)
listbox = Listbox(top, selectmode=tkinter.SINGLE)
listbox.grid(column=0, row=0)
for ind, item in enumerate(mla_list):
    listbox.insert(ind, 'tree{}'.format(ind+1))


def onFrameConfigure(event, canvas):
    '''Reset the scroll region to encompass the inner frame'''
    canvas.configure(scrollregion=canvas.bbox("all"))


def learn_cmd(indices):
    trees = [mla_list[ind][0] for ind in indices]
    teacher = SimpleTeacher()
    for tree in trees:
        teacher.addPositiveExample(tree)
    cfg = learn(teacher, d)
    print(cfg)



out_frame = Frame(top)
out_frame.grid(column=0, row=2)
c = Checklist(range(len(mla_list)), out_frame)
c.pack()
b = Button(out_frame, pady=10, text='OK', command=lambda: learn_cmd(c.get_selected()))
b.pack()
tree_frame = Frame(top)
tree_frame.grid(column=1, row=0, rowspan=2)
cf = CanvasFrame(tree_frame)
cf.canvas()['width'] = 1000
cf.pack()


def draw_tree(tree, cf):
    cf.canvas().delete('all')
    tc = TreeWidget(cf.canvas(), tree, xspace=50, yspace=50,
                    line_width=2, node_font=('helvetica', -28))

    def show_description(sth):
        t = sth.__repr__()
        t = int(t[t.find('\'')+1:t.find(']')-1])
        text['text'] = d[t]
    tc.bind_click_nodes(show_description, button=1)
    cf.add_widget(tc, 200, 0)


def onselect(evt):
    w = evt.widget
    index = int(w.curselection()[0])
    tree = mla_list[index][0]
    draw_tree(tree, cf)


listbox.bind('<<ListboxSelect>>', onselect)
top.mainloop()
