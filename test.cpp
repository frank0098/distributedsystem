#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <stdio.h> 
#include <math.h>
#include <string.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>  
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "assert.h"
#include <stdio.h>
#include <set>
#include <utility>
#include <queue>
using namespace std;
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>=(b))?(a):(b))
#define MAXINT 0X3f3f3f3f

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
 struct TreeNode
 {
     int val;
     TreeNode *left;
     TreeNode *right;
     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 };
class Solution {
public:
    // TreeNode* ret;
    TreeNode* inorderSuccessor(TreeNode* root, TreeNode* p) {
        stack<TreeNode*> stk;
        TreeNode* cur=root;
        stk.push(cur);
        while(cur!=p){
            if(cur->val<p->val){
                cur=cur->right;
            }
            else{
                cur=cur->left;
            }
            stk.push(cur);
        }
        if(cur->right!=NULL){
            cur=cur->right;
            while(cur->left){
                cur=cur->left;
            }
            return cur;
        }
        cout<<"sze"<<stk.size()<<endl;
        TreeNode* prev=stk.top();
        stk.pop();
        if(stk.empty()) return NULL;
        cur=stk.top();
        stk.pop();
        cout<<cur->val<<endl;
        cout<<"stk"<<stk.size()<<endl;

        while(!stk.empty() && cur->right==prev){
            cout<<"g"<<endl;
            prev=cur;
            cur=stk.top();
            stk.pop();
        }
        if(cur->right==prev) return NULL;
        return cur;
        
        
        
        // if(root==NULL) return NULL;
        // if(root->val<=p->val){
        //     return inorderSuccessor(root->right,p);
        // }
        // else{
        //     TreeNode* left=inorderSuccessor(root->left,p);
        //     return left==NULL?root:left;
        // }
    }

};

int main(int argc, char* argv[])
{
    Solution aa;
	TreeNode node1(1);
    TreeNode node2(2);
    TreeNode node3(3);
    TreeNode node4(4);
    node1.right=&node2;
    node3.left=&node1;
    node3.right=&node4;
    cout<<aa.inorderSuccessor(&node3,&node2)->val<<endl;

	return 0;
}



        stack<int> stk;
        for(int i=0;i<s.size();i++){
            if(s[i]=='(') stk.push(i);
            else{
                if(stk.empty()) stk.push(i);
                else{
                    if(s[stk.top()]=='('){
                        stk.pop();
                    }
                    else stk.push(i);
                }
                
            }
        }
        if(stk.empty()) return s.size();
        int end=s.size();
        int ret=0;
        while(!stk.empty()){
            ret=max(ret,end-stk.top()-1);
            end=stk.top();
            stk.pop();
        }
        ret=max(ret,end);
        return ret;

