import CFGLearner
from nltk import Tree


a = Tree(2, [Tree(1, []), Tree(1, [])])
l = []
for c in str(a):
    if c == '(':
        l.append(-1)
        continue
    if c == ')':
        l.append(-2)
        continue
    if c != ' ':
        l.append(int(c))
l = l[1:-1]
print(l)
CFGLearner.addTree(l, True)