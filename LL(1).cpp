#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <iomanip>
#include <stack>
using namespace std;
// 定义文法的结构体
struct Grammar {
    string value; // 非终结符
    vector<vector<string> > right; // 产生式右部
};
string Start;
map<string,set<string>>FIRST,FOLLOW;
set<string>TERMINAL,NON_TERMINAL;
map<pair<string, string>, vector<string> > LL1_Table; // LL(1)分析表
void print(vector<Grammar>& gramma){
    int l=gramma.size();
    for(int i=0;i<l;i++)
    {
        cout<<gramma[i].value<<"-->";
        for(auto right:gramma[i].right)
        {
            for(auto rightt:right){
                cout<<rightt<<' ';
            }
            cout<<"|";
        }
        cout<<"\n";
    }
}
void GetStartSymbol(vector<Grammar>& grammars)
{
    Start=grammars[0].value;
    return;
}
// 消除间接左递归的函数
// 消除间接左递归的函数
void RemoveLeftRecursion(vector<Grammar>& grammars) {
    int length = grammars.size();
    vector<Grammar> newgrammars; // 存储新的文法规则
    for (int i = 0; i < length; ++i) {
        for (int j = 0; j < i; ++j) {
            vector<vector<string>> newright;
            // 对于当前产生式中以前面文法规则非终结符开头的情况
            for (auto& current : grammars[i].right) {
                if (!current.empty() && current[0] == grammars[j].value) {
                    current.erase(current.begin()); // 删除当前产生式中的左递归部分
                    // 将前面文法规则的右部添加到当前产生式的右部
                    for (auto& sub : grammars[j].right) {
                        vector<string> tmp = current;
                        // 将前面文法规则的右部反转并加入到当前产生式的右部
                        for (auto it = sub.rbegin(); it != sub.rend(); it++) {
                            tmp.insert(tmp.begin(), *it);
                        }
                        newright.push_back(tmp);
                    }
                } else {
                    newright.push_back(current); // 如果不是以前面文法规则非终结符开头，则保持不变
                }
            }
            grammars[i].right = newright; // 更新当前文法规则的右部
        }

        // 分割含有左递归的产生式和不含左递归的产生式
        vector<vector<string>> withRecursion;
        vector<vector<string>> withoutRecursion;
        for (auto& production : grammars[i].right) {
            if (production[0] == grammars[i].value) {
                withRecursion.push_back(production); // 含有左递归的产生式
            } else {
                withoutRecursion.push_back(production); // 不含左递归的产生式
            }
        }

        // 如果含有左递归的产生式不为空，则进行左递归消除
        if (!withRecursion.empty()) {
            string newSymbol = grammars[i].value + "~"; // 新的非终结符
            for (auto& temp : withoutRecursion) {
                temp.push_back(newSymbol); // 将不含左递归的产生式末尾加上新的非终结符
            }
            for (auto& temp : withRecursion) {
                temp.erase(temp.begin()); // 删除左递归部分
                temp.push_back(newSymbol); // 在左递归部分后加上新的非终结符
            }
            vector<string> epsilon; // 空串
            epsilon.push_back("#");
            withRecursion.push_back(epsilon); // 含有左递归的产生式最后加上空串
            newgrammars.push_back({newSymbol, withRecursion}); // 添加新的文法规则
            grammars[i].right = withoutRecursion; // 更新当前文法规则的右部，不含左递归的产生式
        }
    }
    // 将新的文法规则加入原文法规则集合中
    grammars.insert(grammars.end(), newgrammars.begin(), newgrammars.end());
}


//消除直接左递归

// void RemoveLeftRecursion(vector<Grammar>& grammars) {
//     int length = grammars.size();
//     vector<Grammar> newGrammars; // 用于存储新的文法规则

//     // 遍历所有文法规则
//     for (int i = 0; i < length; ++i) {
//         vector<vector<string>> directLeftRecursionProductions; // 存储直接左递归产生式
//         vector<vector<string>> remainingProductions; // 存储剩余产生式

//         // 分离直接左递归产生式和剩余产生式
//         for (auto& production : grammars[i].right) {
//             if (!production.empty() && production[0] == grammars[i].value) {
//                 directLeftRecursionProductions.push_back(production); // 存储直接左递归产生式
//             } else {
//                 remainingProductions.push_back(production); // 存储剩余产生式
//             }
//         }

//         if (!directLeftRecursionProductions.empty()) {
//             // 构造新的非终结符，用于替代直接左递归
//             string newSymbol = grammars[i].value + "~";
//             vector<vector<string>> newProductions;

//             // 添加新产生式，替代直接左递归
//             for (auto& production : remainingProductions) {
//                 production.push_back(newSymbol); // 将新的非终结符加入产生式尾部
//                 newProductions.push_back(production); // 添加到新的产生式列表中
//             }



//             // 添加新的文法规则
//             newGrammars.push_back({grammars[i].value, newProductions}); // 将原始非终结符的产生式替换为新的产生式列表

//             // 构造新的非终结符产生式，去除直接左递归
//             vector<vector<string>> newLeftRecursionProductions;
//             for (auto& production : directLeftRecursionProductions) {
//                 production.erase(production.begin()); // 删除非终结符本身
//                 production.push_back(newSymbol); // 替换成新的非终结符
//                             // 添加空产生式
//             vector<string> epsilonProduction = {"#"}; // 创建空产生式
//             // epsilonProduction.push_back(newSymbol); // 将新的非终结符加入到空产生式尾部
//             newProductions.push_back(epsilonProduction); // 添加到新的产生式列表中
//                 newLeftRecursionProductions.push_back(production); // 添加到新的产生式列表中
//                 newLeftRecursionProductions.push_back(epsilonProduction);
//             }

//             // 添加新的文法规则
//             newGrammars.push_back({newSymbol, newLeftRecursionProductions}); // 添加新的文法规则到新的文法列表中
//         } else {
//             // 如果没有左递归，保持原样
//             newGrammars.push_back(grammars[i]); // 添加原始文法规则到新的文法列表中
//         }
//     }

//     // 更新文法规则
//     grammars = newGrammars; // 将新的文法规则列表赋值给原始文法规则列表
// }

// 提取左因子
void extractLeftFactor(vector<Grammar>& grammars) {
    vector<Grammar> newGrammars; // 用于存储新产生的文法

    for (auto& grammar : grammars) {
        map<string, vector<vector<string>>> prefixMap;

        // 寻找共同前缀
        for (const auto& production : grammar.right) {
            string prefix = production[0]; // 取第一个字符作为前缀
            prefixMap[prefix].push_back(production);
        }

        // 处理具有共同前缀的产生式
        vector<vector<string>> newRight;
        for (auto& entry : prefixMap) {
            if (entry.second.size() > 1) { // 发现共同前缀
                string newNonTerminal = grammar.value + "~"; // 创建新非终结符
                vector<string>tmp;
                tmp.push_back(entry.first);
                tmp.push_back(newNonTerminal);
                newRight.push_back(tmp); // 添加新产生式

                // 将原有产生式替换为新的非终结符
                Grammar newGrammar;
                newGrammar.value = newNonTerminal;
                for (auto& prod : entry.second) {
                    if (prod.size() > 1) {
                        prod.erase(prod.begin());
                        newGrammar.right.push_back(prod);
                    } else {
                        vector<string>t;
                        t.push_back("#");
                        newGrammar.right.push_back(t); // 空产生式
                    }
                }
                newGrammars.push_back(newGrammar); // 添加到新文法列表
            } else {
                newRight.push_back(entry.second[0]); // 无需更改
            }
        }

        // 更新原文法的产生式
        grammar.right = newRight;
    }

    // 将所有新文法添加到原文法列表中
    grammars.insert(grammars.end(), newGrammars.begin(), newGrammars.end());
    
}
// 计算每个非终结符的FIRST集合
void getFIRST(vector<Grammar>& grammars)
{
    int len = grammars.size(); // 获取文法规则的数量
    bool change = true; // 是否有新的FIRST集合元素添加标志

    // 遍历文法规则，将终结符加入FIRST集合
    for (auto grammar : grammars) {
        string value = grammar.value; // 获取非终结符名称
        for (auto right : grammar.right) {
            auto r = right[0]; // 获取产生式右部的第一个符号

            // 如果第一个符号是终结符或空串，则将其加入FIRST集合
            if (TERMINAL.count(r) || r == "#") {
                FIRST[value].insert(r);
            }
        }
    }

    // 循环计算每个非终结符的FIRST集合
    while (change) {
        change = false; // 初始化change为false，如果有新的元素添加，则置为true

        // 遍历文法规则
        for (auto grammar : grammars) {
            string value = grammar.value; // 获取非终结符名称
           

            // 遍历文法规则的产生式
            for (auto right : grammar.right) {
                bool episol = true; // 是否存在空串产生式标志
                for(auto rr:right)
                {
                    if(!FIRST[rr].count("#"))
                    episol=false;// 至少一个符号不能推导出空串
                    for(auto ff:FIRST[rr])
                    {
                        if(ff!="#"&&FIRST[value].insert(ff).second){
                            change=true;                          
                        }
                    }
                    if(!episol)
                    break;
                }
                
                if (episol) {
                    FIRST[value].insert("#");
                }
            }
        }
    }
}

// 获取终结符集合
void GetTerminal(vector<Grammar>& grammars)
{
    int len = grammars.size(); // 获取文法规则的数量

    // 将所有非终结符加入NON_TERMINAL集合中
    for (int i = 0; i < len; i++) {
        NON_TERMINAL.insert(grammars[i].value);
    }

    // 遍历文法规则，将产生式中的终结符加入TERMINAL集合中
    for (int i = 0; i < len; i++) {
        auto grammar = grammars[i].right; // 获取文法规则的产生式列表
        for (auto vec : grammar) {
            for (auto str : vec) {
                // 如果产生式符号不在非终结符集合中且不是空串，则将其加入终结符集合
                if (NON_TERMINAL.find(str) == NON_TERMINAL.end() && str != "#") {
                    TERMINAL.insert(str);
                }
            }
        }
    }

    // 添加结束符号$
    TERMINAL.insert("$");
}

// 计算每个非终结符的FOLLOW集合
void GetFollow(vector<Grammar>& grammars)
{
    bool changed = true; // 是否有新的FOLLOW集合元素添加标志
    FOLLOW[Start].insert("$"); // 开始符号的FOLLOW集合中加入结束符号$

    // 循环计算每个非终结符的FOLLOW集合
    while (changed) {
        changed = false; // 初始化changed为false，如果有新的元素添加，则置为true

        // 遍历文法规则
        for (auto& grammar : grammars) {
            for (auto& production : grammar.right) {
                int productionSize = production.size(); // 获取产生式的长度
                for (int i = 0; i < productionSize; i++) {
                    if (NON_TERMINAL.count(production[i]) > 0) { // 如果产生式符号是非终结符
                        int oldSize = FOLLOW[production[i]].size(); // 获取FOLLOW集合原始大小
                        if (i < productionSize - 1) {
                            if (TERMINAL.count(production[i + 1]) > 0) { // 如果下一个符号是终结符
                                FOLLOW[production[i]].insert(production[i + 1]); // 直接将其加入FOLLOW集合
                            } else { // 如果下一个符号是非终结符
                                int j = i + 1;
                                bool allEpsilon = true; // 是否全为空串产生式标志
                                while (j < productionSize && allEpsilon) {
                                    allEpsilon = false; // 初始化标志为false
                                    for (auto& first : FIRST[production[j]]) { // 遍历下一个符号的FIRST集合
                                        if (first != "#") {
                                            FOLLOW[production[i]].insert(first); // 将非空串符号加入FOLLOW集合
                                        } else {
                                            allEpsilon = true; // 如果存在空串符号，则继续遍历下一个符号
                                        }
                                    }
                                    j++;
                                }
                                if (allEpsilon) { // 如果全部为空串产生式
                                    for (auto& follow : FOLLOW[grammar.value]) {
                                        FOLLOW[production[i]].insert(follow); // 将文法规则的FOLLOW集合加入当前非终结符的FOLLOW集合
                                    }
                                }
                            }
                        } else { // 如果是产生式的最后一个符号
                            for (auto& follow : FOLLOW[grammar.value]) {
                                FOLLOW[production[i]].insert(follow); // 将文法规则的FOLLOW集合加入当前非终结符的FOLLOW集合
                            }
                        }
                        if (FOLLOW[production[i]].size() != oldSize) { // 如果FOLLOW集合有新的元素添加，则changed置为true
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}


// 构建LL(1)分析表
void buildLL1Table(vector<Grammar>& grammars) {
    // 遍历文法规则
    for (auto& grammar : grammars) {
        string non_terminal = grammar.value; // 获取非终结符名称
        // 遍历产生式
        for (auto& production : grammar.right) {
            set<string> first_set; // 存储当前产生式的first集合
            // 遍历产生式中的符号
            for (auto& symbol : production) {
                // 处理空串产生式的情况
                if (production.size() == 1 && symbol == "#") {
                    first_set.insert(symbol);
                    break;
                }
                // 如果是终结符，则将其加入first集合，并退出循环
                if (TERMINAL.count(symbol) > 0) {
                    first_set.insert(symbol);
                    break;
                }
                // 如果是非终结符，则将其FIRST集合中的终结符加入first集合
                else if (NON_TERMINAL.count(symbol) > 0) {
                    for (auto& terminal : FIRST[symbol]) {
                        first_set.insert(terminal);
                    }
                    // 如果FIRST集合中不包含#，则退出循环
                    if (FIRST[symbol].count("#") == 0) {
                        break;
                    }
                }
            }
            // 将first集合中的终结符对应的产生式加入LL(1)分析表
            for (auto& terminal : first_set) {
                LL1_Table[{non_terminal, terminal}] = production;
            }
            // 如果first集合中包含#，则将FOLLOW集合中的终结符对应的产生式也加入LL(1)分析表
            if (first_set.count("#") > 0) {
                for (auto& terminal : FOLLOW[non_terminal]) {
                    LL1_Table[{non_terminal, terminal}] = production;
                }
            }
        }
    }
}


// 打印LL(1)分析表
// void printLL1Table() {
//     // 获取所有非终结符和终结符
//     vector<string> non_terminals(NON_TERMINAL.begin(), NON_TERMINAL.end());
//     vector<string> terminals(TERMINAL.begin(), TERMINAL.end());

//     // 打印表头
//     cout << left<<setw(18) << " ";
//     for (const auto& terminal : terminals) {
//         cout << left<<setw(18) << terminal;
//     }
//     cout << endl;

//     // 打印表格内容
//     for (const auto& non_terminal : non_terminals) {
        
//         cout << left<<setw(18)<<non_terminal;
//         for (const auto& terminal : terminals) {
//             string tmp=non_terminal;
//             auto production = LL1_Table[{non_terminal, terminal}];
//             if (!production.empty()) {
//                 tmp+="->";
//                 // cout<<left<<setw(18)<<non_terminal;
//                 // cout<<"->"
//                 for (const auto& symbol : production) {
//                     // cout<< symbol<<" ";
//                     tmp+=symbol+" ";
//                 }
//                 cout<<setw(18)<<tmp;
//                 // cout<<endl;
//             } else {
//                 cout << left<<setw(18) << "null";
//             }
//         }
//         cout << endl;
//     }
// }
// 输出LL(1)分析表的函数
void printLL1Table() {
    // 为了保证输出的顺序，我们对终结符进行排序
    vector<string> sortedTerminals(TERMINAL.begin(), TERMINAL.end());
    sort(sortedTerminals.begin(), sortedTerminals.end());

    // 打印表头
    cout << setw(20) << left << "Non-Terminals" << "|";
    for (const string& terminal : sortedTerminals) {
        cout << setw(20) << left << terminal << "|";
    }
    cout << endl;

    // 用于分隔的线
    cout << string(21 + 21 * sortedTerminals.size(), '-') << endl;

    // 打印表内容
    for (const string& non_terminal : NON_TERMINAL) {
        cout << setw(20) << left << non_terminal << "|";
        for (const string& terminal : sortedTerminals) {
            auto it = LL1_Table.find({non_terminal, terminal});
            if (it != LL1_Table.end()) {
                string productionStr = "";
                productionStr+=non_terminal+"->";
                for (const string& sym : it->second) {
                    productionStr += sym + " ";
                }
                cout << setw(20) << left << productionStr << "|";
            } else {
                cout << setw(20) << left << " " << "|";
            }
        }
        cout << endl;
    }
}
Grammar getGrammar()
{
    Grammar gramm;
    vector<vector<string>>right;
    vector<string>pro;
    string left;
    string tmp;

        getline(cin,tmp);
        
        string t;
        bool r=false,l=true;
        for(int i=0;i<tmp.size();i++)
        {
            char a=tmp[i];
            if(a!='-'&&l)
            {t+=a;continue;}
            else if(a=='-'&&l){left=t;t.clear();l=!l;continue;}
            if(a=='>'){r=true;continue;}
            if(r){
                if(a==' '){
                    pro.push_back(t);
                    t.clear();continue;
                }
                if(a=='|'){
                    right.push_back(pro);
                    pro.clear();continue;
                }
                t+=a;
                if(i==tmp.size()-1){
                    pro.push_back(t);
                    right.push_back(pro);
                }
            }
        }
    
    gramm.value=left;
    gramm.right=right;
    return gramm;
    
}
void printStack(stack<string>s)
{
    while(!s.empty())
    {
        cout<<" ";
        cout<<s.top();s.pop();
    }
}

int main() {
    // 示例文法
    // vector<Grammar> grammars = {
    //     {"S", {{"a"}, {"b"},{"(","T",")"}}}
    //     ,
    //     {"T", {{"T", ":", "S"}, {"S"}}}
    // };
    vector<Grammar>grammars;
    cout<<"输入产生式个数：";
    int n;cin>>n;
    cin.ignore();
    for(int i=0;i<n;i++)
    {
        Grammar g=getGrammar();
        grammars.push_back(g);
    }
    GetStartSymbol(grammars);
    RemoveLeftRecursion(grammars);
    extractLeftFactor(grammars); print(grammars);
    // 调用相关函数
    GetTerminal(grammars);
    getFIRST(grammars);
    GetFollow(grammars);
   
    buildLL1Table(grammars);
    stack<string>parsingStack,inputstack,tmp;
    // 输出LL(1)分析表
    cout << "LL(1) Analysis Table:\n";
    printLL1Table();
    cout<<"输入要分析的串";

    
    // cout<<setw(20)<<"分析栈"<<"输入"<<"动作"<<endl;
    string s;
    while(cin>>s)tmp.push(s);
    inputstack.push("$");
    while(!tmp.empty())
    {
        inputstack.push(tmp.top());
        tmp.pop();
    }
    parsingStack.push("$");
    parsingStack.push(Start);
    cout << setw(34) << left << "分析栈" << setw(34) << "输入" << setw(34) << "动作" << endl;
    cout << string(90, '-') << endl; 
while (!parsingStack.empty()) {
    printStack(parsingStack);
    cout << setw(30 - parsingStack.size() * 2); // 根据栈内容调整间距
    printStack(inputstack);

    cout << setw(30 -inputstack.size() * 2); // 根据栈内容调整间距

    // 如果栈顶非终结符和输入符号相同，则匹配，移除栈顶元素和输入栈顶元素
    if (parsingStack.top() == inputstack.top()) {
        if (parsingStack.top() == "$") {
            cout << ""<<"接受" << endl;
            exit(0);
        }
        parsingStack.pop();
        inputstack.pop();
        cout << ""<<"匹配" << endl;
    } else {
        // 否则，在LL(1)分析表中查找对应的产生式
        string non_terminal = parsingStack.top();
        string terminal = inputstack.top();

        if (LL1_Table.find({non_terminal, terminal}) != LL1_Table.end()) {
            // 找到对应的产生式
            auto production = LL1_Table[{non_terminal, terminal}];

            // 将栈顶的非终结符移除
            parsingStack.pop();

            // 将产生式右部逆序压入栈中
            for (auto it = production.rbegin(); it != production.rend(); ++it) {
                if (*it != "#") {
                    parsingStack.push(*it);
                }
            }

            // 打印动作
            cout << "" << non_terminal << " -> ";
            for (const auto& symbol : production) {
                cout << symbol << " ";
            }
            cout << endl;
        } else {
            // 找不到对应的产生式，输入串不符合文法规则
            cout <<""<< "error" << endl;
            exit(0);
        }
    }
}}
