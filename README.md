# LL-1-parsing
Complete the LL(1) analysis of compiler theory, including eliminating direct left recursion, in-direct left recursion, and extracting left factors.
输入样例：
![image](https://github.com/WUXI666666/LL-1-parsing/assets/128573257/b51d20e0-639f-4eaa-aaf3-8073abc91c9c)

样例输入：
输入产生式个数：2

S->a |b |( T )

T->T : S |S

注意每个符号后面都需要加一个空格为了区分是否属于不同的符号
