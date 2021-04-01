#include <iostream>
#include <cmath>
#include <map>
#include <list>
#include <algorithm>
#include <bits/stdc++.h>
using namespace std;

class node {
public:
    bool leaf;

    vector<pair<int,double>> k; // k stores the data value pairs(int for the keys and double for the values). if the node is an internal node, the value of the double in a pair will be -1
    int capacity; //capacity of k
    //int* k, capacity; //key stores the data values in the node, capacity is the # of data values currently in the node 
    node** a; // a list of children
    node* prev; 
    node* next;
    node(int m);
};
node::node(int m) { 
    leaf = true;
    prev = nullptr;
    next = nullptr;
    capacity = 0;
    a = new node * [m + 1]; //a node can have a maximum of m+1 children
    for(int i = 0; i < m+1;i++){// intialize the list of children to nullptr
        a[i] = nullptr;
    }
}
class bplustree {
public:
    node* root;
    node* lastInsertedLeaf;
    vector<pair<int,node*>> ll; 
    int m;
    int min_for_nonleaf;
    int min_for_leaf;
    int min_children;
    void Intialize(int x);
    void Insert(int key, double val);
    double Search(int key);
    void InsertInternal(stack<node*> s,node* temp, int key, double val);
    bool biasChecker(node* n);
    int findInsertionChild(int key,node* temp, bool bias);
};


void bplustree::Intialize(int x) {
    m = x;
    double mdouble = m;
    min_for_nonleaf = ceil(mdouble/2);
    min_for_leaf = ceil((mdouble-1)/2);
    min_children = min_for_nonleaf - 1;
    lastInsertedLeaf = nullptr;
    root = NULL; 

}
bool bplustree::biasChecker(node* n){ // this function checks if the true is left biased or right biased, returns true if right biased, false if left biased or neither
    bool bias = false;
    int max_k_size = 0;
    int position =0;
    for(int i = 0; i < n->capacity; i++ ){
        if(n->a[i]->k.size() > max_k_size){
            max_k_size = n->a[i]->k.size();
            position = i;
        }
    }
    if(position > 0){
        bias = true;
    }else if(position == 0){
        bias = false;
    }
    return bias;
}
int bplustree::findInsertionChild(int key, node* temp, bool bias){ // this function finds the spot where the node should be inserted 
    int position = 0;
    if(bias == false){
        for(int i = 0; i < temp->capacity-1;i++){
        if(temp->a[i]->k[0].first > key){
                position = i; 
                break;
                       }
    }
    if(position == 0){
        return 0;
    }else return position -1;
    }else {
        for(int i = temp->capacity-2; i > 0 ;i--){
        if(temp->a[i]->k[0].first <= key){
                position = i; 
                break;
                       }
    }
     return position;
    }
   
}
void bplustree::InsertInternal(stack<node*> s,node* temp, int key, double val){
            node* leftSplit = new node(3); 
            node* parent = s.top();
            s.pop();
            for(int i = 0; i < min_children; i++){ // left node in the split will contain ceil((m-1)/2)-1 values. erase these from temp
                leftSplit->k.push_back(make_pair(temp->k[0].first,0));
                temp->k.erase(temp->k.begin());
            }
            leftSplit->leaf = false;
            temp->leaf = false;
            vector<node*> left;
            vector<node*> right;
                int insertionPoint = parent->capacity/2;
                for(int i = 0;i < insertionPoint; i++){ // collect the children that should be left of the insertionPoint
                    if(insertionPoint == 0){ // if the insertion point is at the far left break, there are no left children
                        break;
                    }
                    left.push_back(parent->a[i]);
                }     
                for(int i = insertionPoint;i < parent->capacity; i++){ // collect the children that should be right of the insertion point
                    if(i != 0){ // if the insertionPoint is 0 there are no children that must be collected
                        right.push_back(parent->a[i]); 
                    }
                    
                }   
              if(s.empty() == true){ // if the root was the originally overfull node
                node* newRoot = new node(3);
                newRoot->k.push_back(make_pair(temp->k[0].first,0)); // copy the smallest search key value from the second node to the parent
                temp->k.erase(temp->k.begin());
                sort(newRoot->k.begin(),newRoot->k.end());           
                newRoot->a[0] = leftSplit;
                newRoot->a[1] = temp;
                newRoot->leaf = false;
                root = newRoot;
                newRoot->capacity+=2;
                for(int i = 0; i < parent->capacity/2; i++){
                    leftSplit->a[i] = left[0];
                    left.erase(left.begin());
                    temp->a[i] = right[0];
                    right.erase(left.begin());
                }
                temp->capacity=0;
                leftSplit->capacity+=(parent->capacity/2);
                temp->capacity+=(parent->capacity/2);
                if(parent->k.size() == m){
                    InsertInternal(s,parent,key,val);
                }

            }else{
                node* gp = s.top(); // the parent of the current parent
                gp->k.push_back(make_pair(temp->k[0].first,0)); // copy the smallest search key value from the second node to the parent
                temp->k.erase(temp->k.begin()); // remove the copy from the second node
                sort(gp->k.begin(),gp->k.end());  
                gp->capacity+=1;
                bool bias = biasChecker(gp);
                int insertionPoint = findInsertionChild(temp->k[0].first,gp,bias);  
                vector<node*> leftGP;
                vector<node*> rightGP;

                for(int i = 0;i < insertionPoint; i++){ // collect the children that should be left of the insertionPoint
                    if(insertionPoint == 0){ // if the insertion point is at the far left break, there are no left children
                        break;
                    }
                    leftGP.push_back(gp->a[i]);
                }     
                for(int i = insertionPoint;i < gp->capacity-1; i++){ // collect the children that should be right of the insertion point
                    if(i != 0){ // if the insertionPoint is 0 there are no children that must be collected
                        rightGP.push_back(gp->a[i]);
                    }
                    
                }   
                for(int i = 0; i<gp->capacity; i++){
                    if(i == insertionPoint){ // if at insertion point put the leftSplit
                        gp->a[i] = leftSplit;
                    }else if( i == insertionPoint + 1){ // temp always follows leftSplit
                        gp->a[i] = temp;
                    }else if(i < insertionPoint){ // if i is less than the insertion point set a left child
                        gp->a[i] = leftGP[0]; 
                        leftGP.erase(leftGP.begin());
                    }else if(i > insertionPoint){ // if i is greater than the insertion point set a right child
                        gp->a[i] = rightGP[0];
                        rightGP.erase(rightGP.begin());
                    }

                }
                temp->leaf = false; 
                leftSplit->leaf = false;
                for(int i = 0; i < parent->capacity/2; i++){ // setting the children of leftSplit and temp
                    leftSplit->a[i] = left[0];
                    left.erase(left.begin());
                    temp->a[i] = right[0];
                    right.erase(left.begin());
                }
                temp->capacity=0;
                leftSplit->capacity+=(parent->capacity/2);
                temp->capacity+=(parent->capacity/2);
                if(gp->k.size() == m){
                    InsertInternal(s,gp,key,val);
                }

            }
        
        
}


void bplustree::Insert(int key, double val){
    if(root == NULL){
        root = new node(m);
        root->k.push_back(make_pair(key,val));
        return;
    }else if(Search(key) != -1){
        return;
    }else{
        stack<node*> s;
        node* temp = root;
        node* parent = temp;
        while(temp->leaf != true){
            s.push(temp);
            parent = temp;
            for(int i = 0; i < temp->k.size(); i++){ 
                if(key < temp->k[i].first){  // if the searchKey is less than the current key go to the left child of the current node
                    temp = temp->a[i];
                    break;
                }
                if( i == temp->k.size()-1){ //if the searchKey is >= the current key and there are no more keys at this node, go the right subchild of the current node
                    temp = temp->a[i+1];
                    break;
                }
            }
        }
       
        if(temp->k.size() < m-1){ //the leaf node has room for another entry
            temp->k.push_back(make_pair(key,val));
            sort(temp->k.begin(),temp->k.end());
        }else if(temp->k.size() == m-1 && biasChecker(parent) == false){ // if a leaf is about to be overfull and be left biased
            temp->k.push_back(make_pair(key,val)); // add the new value to the now overfull node
            sort(temp->k.begin(),temp->k.end()); 
            node* leftSplit = new node(3); 
            for(int i = 0; i < min_for_leaf; i++){ // left node in the split will contain ceil((m-1)/2) values. erase these from temp
                leftSplit->k.push_back(make_pair(temp->k[0].first,temp->k[0].second));
                temp->k.erase(temp->k.begin());
            }
            temp->prev = leftSplit; // adjust the leaf pointers
            leftSplit->next = temp;
            
              if(parent->a[0] == nullptr){ // if the root was the originally overfull node
                node* newRoot = new node(3);
                newRoot->k.push_back(make_pair(temp->k[0].first,0)); // copy the smallest search key value from the second node to the parent
                sort(newRoot->k.begin(),newRoot->k.end());           
                newRoot->a[0] = leftSplit;
                newRoot->a[1] = temp;
                newRoot->leaf = false;
                root = newRoot;
                newRoot->capacity+=2;
            }else if(parent->capacity >= 2 ){ // if the parent has two or more children
                parent->k.push_back(make_pair(temp->k[0].first,0)); // copy the smallest search key value from the second node to the parent
                sort(parent->k.begin(),parent->k.end());          
                parent->capacity+=1; 
                 int insertionPoint = findInsertionChild(temp->k[0].first,parent,false);  
                vector<node*> left;
                vector<node*> right;

                for(int i = 0;i < insertionPoint; i++){ // collect the children that should be left of the insertionPoint
                    if(insertionPoint == 0){ // if the insertion point is at the far left break, there are no left children
                        break;
                    }
                    left.push_back(parent->a[i]);
                }     
                for(int i = insertionPoint;i < parent->capacity-1; i++){ // collect the children that should be right of the insertion point
                    if(i != 0){ // if the insertionPoint is 0 there are no children that must be collected
                        right.push_back(parent->a[i]);
                    }
                    
                }   
                for(int i = 0; i<parent->capacity; i++){
                    if(i == insertionPoint){ // if at insertion point put the leftSplit
                        parent->a[i] = leftSplit;
                    }else if( i == insertionPoint + 1){ // temp always follows leftSplit
                        parent->a[i] = temp;
                    }else if(i < insertionPoint){ // if i is less than the insertion point set a left child
                        parent->a[i] = left[0]; 
                        left.erase(left.begin());
                    }else if(i > insertionPoint){ // if i is greater than the insertion point set a right child
                        parent->a[i] = right[0];
                        right.erase(right.begin());
                    }

                }
        }
        }else if(temp->k.size() == m-1 && biasChecker(parent) == true){ // if a lead is about to be overfull and be right biased
             temp->k.push_back(make_pair(key,val)); // add the new value to the now overfull node
            sort(temp->k.begin(),temp->k.end()); 
            node* leftSplit = new node(3); 
            for(int i = 0; i < min_for_leaf; i++){ // left node in the split will contain ceil((m-1)/2) values. erase these from temp
                leftSplit->k.push_back(make_pair(temp->k[0].first,temp->k[0].second));
                temp->k.erase(temp->k.begin());
            }
            temp->prev = leftSplit; // adjust the leaf pointers
            leftSplit->next = temp;
              if(parent->a[0] == nullptr){ // if the root was the originally overfull node
                node* newRoot = new node(3);
                newRoot->k.push_back(make_pair(temp->k[0].first,0)); // copy the smallest search key value from the second node to the parent
                sort(newRoot->k.begin(),newRoot->k.end());           
                newRoot->a[0] = leftSplit;
                newRoot->a[1] = temp;
                newRoot->leaf = false;
                root = newRoot;
                newRoot->capacity+=2;
            }else if(parent->capacity >= 2 ){ // if the parent has two or more children
                parent->k.push_back(make_pair(temp->k[0].first,0)); // copy the smallest search key value from the second node to the parent
                sort(parent->k.begin(),parent->k.end());          
                parent->capacity+=1; 
                 int insertionPoint = findInsertionChild(temp->k[0].first,parent,true);  
                vector<node*> left;
                vector<node*> right;

                for(int i = 0;i < insertionPoint; i++){ // collect children that would be left of the insertion point
                    if(insertionPoint == 0){
                        break;
                    }
                    left.push_back(parent->a[i]);
                }     
                for(int i = insertionPoint;i < parent->capacity-1; i++){ // collect children that would be at the right of the insertion point
                    
                    if(parent->a[i]->k[0].first == temp->k[0].first ){ // if the rightmost value is the first key in one of the children nodes skip it
                            continue;
                    }else if(i != 0){ 
                        right.push_back(parent->a[i]);
                    }

                    
                }   
                for(int i = 0; i<parent->capacity; i++){
                    if(i == insertionPoint){
                        parent->a[i] = leftSplit;
                    }else if( i == insertionPoint + 1){
                        parent->a[i] = temp;
                    }else if(i < insertionPoint){
                        parent->a[i] = left[0]; 
                        left.erase(left.begin());
                    }else if(i > insertionPoint){
                        parent->a[i] = right[0];
                        right.erase(right.begin());
                    }

                }
            }
        }
        if(parent->k.size() == m){
            InsertInternal(s,parent,key,val);
        }
}
}
double bplustree::Search(int key){
    if(root == NULL){
        return -1;
    }else{
        node* temp = root;
        while(temp->leaf != true){
            for(int i = 0; i < temp->k.size(); i++){ 
                if(key < temp->k[i].first){  // if the searchKey is less than the current key go to the left child of the current node
                    temp = temp->a[i];
                    break;
                }
                if( i == temp->k.size()-1){ //if the searchKey is >= the current key and there are no more keys at this node, go the right subchild of the current node
                    temp = temp->a[i+1];
                    break;
                }
            }
        }
        for(int i = 0; i < temp->k.size(); i++){
            if(temp->k[i].first == key){ // if the searchKey is found return the corresponding pair to the key
                return temp->k[i].second;
            }
        }
        return -1;
    }
}
int main()
{
    bplustree tree; 
    tree.Intialize(3);
    // tree.Insert(12,12);
    // tree.Insert(108,31.907);
    // tree.Insert(56089,3.26);
    //tree.Insert(234,121.56);
    //tree.Insert(4325,-109); 
   // tree.Insert(30,27.123);  
    tree.Insert(80,80);
    tree.Insert(70,70);
    tree.Insert(60,60);
    tree.Insert(50,50);
    tree.Insert(40,40);
    tree.Insert(30,30); 
    tree.Insert(20,20); 
    tree.Insert(90,90); 
    tree.Insert(100,100); 
    //tree.Insert(10,10);
    // tree.Insert(85,85); 
    // tree.Insert(90,90); 
    // tree.Insert(10,10);
    // tree.Insert(71,71); 
    // tree.Insert(72,72); 
    // tree.Insert(73,73);
    
     
     
      
      
   
  
        
       
     
     
    
    


    cout <<"this should be 80 " <<tree.Search(80)<< endl;
     cout <<"this should be 70 " <<tree.Search(70)<< endl;
     cout <<"this should be 60 " <<tree.Search(60)<< endl;
     cout <<"this should be 50 " <<tree.Search(50)<< endl;
     cout <<"this should be 40 " <<tree.Search(40)<< endl;
     cout <<"this should be 30 " <<tree.Search(30)<< endl;
     cout <<"this should be 20 " <<tree.Search(20)<< endl;
     cout <<"this should be 100 " <<tree.Search(100)<< endl;
       // cout <<"this should be 85 " <<tree.Search(85)<< endl;
     cout <<"this should be 90 " <<tree.Search(90)<< endl;
    //   cout <<"this should be 71 " <<tree.Search(71)<< endl;
    //  cout <<"this should be 72 " <<tree.Search(72)<< endl;
    //   cout <<"this should be 73 " <<tree.Search(73)<< endl;


}