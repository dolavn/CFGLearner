from CFGLearner import SimpleTeacher, Teacher, learn
from nltk import Tree, CFG
import matplotlib as plt
plt.use('Agg')
from nltk.draw import TreeView

a1 = Tree(0, [Tree(1, []), Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(2, [])])])
a2 = Tree(0, [Tree(1, []), Tree(2, [])])
a3 = Tree(0, [Tree(1, []), Tree(0, [Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(2, [])]), Tree(2, [])])])

t1 = Tree(0, [Tree(2, []), Tree(1, [])])
t2 = Tree(0, [Tree(1, []), Tree(2, [])])
t3 = Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(2, []), Tree(1, [])])])
t4 = Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(2, []), Tree(1, [])])])])
t5 = Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(2, []), Tree(1, [])])])])])


p1 = Tree(0, [Tree(1, []), Tree(1, [])])
p2 = Tree(0, [Tree(1, []), Tree(0, [Tree(1, []), Tree(0, [Tree(1, []), Tree(1, [])])])])
p3 = Tree(0, [Tree(1, []), p2])
p4 = Tree(0, [Tree(1, []), p3])
p5 = Tree(0, [Tree(1, []), p4])

T_NUM = 20

pi = [Tree(0, [Tree(1, []), Tree(2, [])])]
for i in range(1, T_NUM):
    pi.append(Tree(0, [Tree(1, []), Tree(0, [pi[i-1], Tree(2, [])])]))


pos = [i for i in range(T_NUM) if i > 1]
t2 = SimpleTeacher()
for i in range(T_NUM):
    if i in pos:
        t2.addPositiveExample(pi[i])
    else:
        t2.addNegativeExample(pi[i])
c = learn(t2)
print(c)