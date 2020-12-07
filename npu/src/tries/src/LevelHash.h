#ifndef __Data_Structure__LevelHash__
#define __Data_Structure__LevelHash__
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "LevelHashEntry.h"
#include "LevelHashBucket.h"
#include "BitString.h"
extern "C" {
#include "../../behavioural/hash.h"
}
//TODO Should not need these, may need to adjust LevelHashEntry to accomodate
#define KEY_LEN 4                        // The maximum length of a key
#define VALUE_LEN 16                      // The maximum length of a value
//incorporated in LevelHashEntry/Bucket
//typedef struct entry{                     // A slot storing a key-value item 
//    uint8_t key[KEY_LEN];
//    uint8_t value[VALUE_LEN];
//    //Pointer to the region
//    uint8_t * storage_ptr;
//} entry;
template <class T>
class LevelHash {
    public:
        // Constructor
        LevelHash(uint64_t level_size, uint64_t hist_len);
        // Destructor
        ~LevelHash();
        uint8_t level_insert(BitString key, T value, int value_size);          
        T level_static_query(BitString key);
        //uint8_t* level_dynamic_query(level_hash *level, uint8_t *key);
        uint8_t level_delete(BitString key);
        uint8_t level_update(BitString key, T new_value);
        void level_expand();
        //unchanged function but changed declaration for compiler
        void level_shrink();
        uint8_t try_movement(uint64_t idx, uint64_t level_num, BitString key, T value, uint8_t *storage_ptr);
        int b2t_movement(uint64_t idx);
        //Getters below
        uint64_t getSeed1() const;
        uint64_t getSeed2() const;
        uint64_t getSize() const;
        //LevelHashBucket<T>[] getMRoot(int level) const;
        
        bool hasResized() const;
        void setResized();

    private:
        //two levels, second dim set at runtime
        std::vector<std::vector<LevelHashBucket<T>>> mRoot;
        //LevelHashBucket<T> *mRoot[2];
        //TODO come back, comes from their hashtrie implementation
        T mDefaultAction;
        // The numbers of items stored in the top and bottom levels respectively
        uint64_t level_item_num[2]; 
        // The number of buckets in the top level
        uint64_t addr_capacity; 
        // The number of all buckets in the Level hash table    
        uint64_t total_capacity;              
        // level_size = log2(addr_capacity)
        uint64_t level_size;                  
        // Indicate whether the Level hash table was expanded, ">1 or =1": Yes, "0": No;
        uint8_t level_expand_time;            
        // Indicate the resizing state of the level hash table, ‘0’ means the hash table is not during resizing; 
        // ‘1’ means the hash table is being expanded; ‘2’ means the hash table is being shrunk.
        uint8_t resize_state;                 
        //Size of MTU for the storage region malloc
        uint64_t mtu_size;
        //Size of stored history before wraparound, needed for storage region
        uint64_t hist_len;
        // Two randomized seeds for hash functions
        uint64_t f_seed;
        uint64_t s_seed;                      
        uint64_t F_HASH(const BitString key);
        uint64_t S_HASH(const BitString key);
        uint64_t F_IDX(uint64_t hashKey, uint64_t capacity);
        uint64_t S_IDX(uint64_t hashKey, uint64_t capacity);
        //void* alignedmalloc(size_t size);
        void generate_seeds();
        bool resized_started;
        //TODO SETTERS AND GETTERS FOR EVERYTHING (TO BE SAFE :D)
};

//#include "level_hashing.h"
//TODO CURRENTLY LEAVING STORAGE PTR AS NULL TO AVOID COMPLICATION
/// ==========================
//
//  Constructors
//
/// ==========================
//We are passing the log of the size(easier)
template <class T>
LevelHash<T>::LevelHash(uint64_t size, uint64_t len) {
    level_size = size;
    //setting the defult to 1500, should not change much
    mtu_size = 1500;
    hist_len = len;
    addr_capacity = (1L << level_size);
    total_capacity = (1L << level_size) + (1L << (level_size-1));

    generate_seeds();
    //create top level 
    //std::vector<LevelHashBucket<T>> topLevel(addr_capacity);
    //std::vector<LevelHashBucket<T>> bottomLevel(addr_capacity/2);
    //mRoot.push_back(topLevel);
    //mRoot.push_back(bottomLevel);
    mRoot.resize(2);
    int i;
    for(i = 0; i < 2; i++) {
        mRoot[i].resize(addr_capacity/(i + 1));
    }
    // mRoot[0] = new LevelHashBucket<T>[addr_capacity];
    // mRoot[1] = new LevelHashBucket<T>[addr_capacity/2];
    //starts off empty
    level_item_num[0] = 0;
    level_item_num[1] = 0;
    level_expand_time = 0;
    resize_state = 0;
    resized_started = true;
    // if (mRoot == NULL)
    // {
    // 	printf("The level hash table initialization fails:2\n");
    // 	exit(1);
    // }
    //sanity print
    cout << "The level hash table initialization succeeds!\n" << endl;
}
/// ==========================
//
//  Destructor
//
/// ==========================
template <class T>
LevelHash<T>::~LevelHash() {
    //Free both levels of the table
    //TODO verify this doesn't leak memory
    delete mRoot[0];
    delete mRoot[1];
}
/*
Function: F_HASH()
Compute the first hash value of a key-value item
*/
//These functions should be changed appropriately, we cast the pointer to the
//first entry in the underlying vector of the bitstring to a void * that the hash function
//wants, this is nice since the bool underlying data type matches the char * that will be 
//used during hashing
template<class T>
uint64_t LevelHash<T>::F_HASH(const BitString key) {
    std::vector<bool> vec = key.getVector();
    return (customHash((void *)(&vec), vec.size(), f_seed));
}
/*
Function: S_HASH() 
Compute the second hash value of a key-value item
*/
template<class T>
uint64_t LevelHash<T>::S_HASH(const BitString key) {
    std::vector<bool> vec = key.getVector();
    return (customHash((void *)(&vec), vec.size(), s_seed));
}
/*
Function: F_IDX() 
Compute the second hash location
*/
template<class T>
uint64_t LevelHash<T>::F_IDX(uint64_t hashKey, uint64_t capacity) {
    return hashKey % (capacity / 2);
}
/*
Function: S_IDX() 
Compute the second hash location
*/
template<class T>
uint64_t LevelHash<T>::S_IDX(uint64_t hashKey, uint64_t capacity) {
    return hashKey % (capacity / 2) + capacity / 2;
}
//template<class T>
//void* LevelHash<T>::alignedmalloc(size_t size) {
//  void* ret;
//  posix_memalign(&ret, 64, size);
//  return ret;
//}
/*
Function: generate_seeds() 
Generate two randomized seeds for hash functions
TODO C++ify this, the random number generator syntax is currently wrong
*/
    template<class T>
void LevelHash<T>::generate_seeds()
{
    //This should be a more robust c++ generator than the default
    mt19937 rnd(time(NULL));
    do
    {
        f_seed = rnd();
        s_seed = rnd();
        f_seed = f_seed << (rnd() % 63);
        s_seed = s_seed << (rnd() % 63);
    } while (f_seed == s_seed);
}
/*
Function: level_expand()
Expand a level hash table in place;
Put a new level on top of the old hash table and only rehash the
items in the bottom level of the old hash table;
*/
    template<class T>
void LevelHash<T>::level_expand() 
{
    resize_state = 1;
    addr_capacity = 1L << (level_size + 1);
    std::vector<LevelHashBucket<T> > newBuckets;
    newBuckets.resize(addr_capacity);
    uint64_t new_level_item_num = 0;
    int ASSOC_NUM = mRoot[0][0].getASSOC();
    uint64_t old_idx;
    for (old_idx = 0; old_idx < (1L << (level_size-1)); old_idx++) {
        uint64_t i, j;
        for(i = 0; i < ASSOC_NUM; i ++){
            if (mRoot[1][old_idx].getToken(i) == 1)
            {
                LevelHashEntry<T> entry = *mRoot[1][old_idx].getSlot(i);
                uint64_t f_idx = F_IDX(F_HASH(entry.getKey()), addr_capacity);
                uint64_t s_idx = S_IDX(S_HASH(entry.getKey()), addr_capacity);
                uint8_t insertSuccess = 0;
                for(j = 0; j < ASSOC_NUM; j ++){                            
                    /*  The rehashed item is inserted into the less-loaded bucket between 
                        the two hash locations in the new level
                        */
                    if (newBuckets[f_idx].getToken(j) == 0)
                    {
                        LevelHashEntry<T>* newEntry = new LevelHashEntry<T>(entry);
                        newBuckets[f_idx].setSlot(j, newEntry);
                        newBuckets[f_idx].setToken(j,1);
                        insertSuccess = 1;
                        new_level_item_num ++;
                        break;
                    }
                    if (newBuckets[s_idx].getToken(j) == 0)
                    {
                        LevelHashEntry<T>* newEntry = new LevelHashEntry<T>(entry);
                        newBuckets[s_idx].setSlot(j, newEntry);
                        newBuckets[s_idx].setToken(j,1);
                        insertSuccess = 1;
                        new_level_item_num ++;
                        break;
                    }
                }
                if(!insertSuccess){
                    printf("The expanding fails: 3\n");
                    exit(1);                    
                }
                mRoot[1][old_idx].setToken(i,0);
            }
        }
    }
    resized_started = true;
    level_size++;
    total_capacity = (1L << level_size) + (1L << (level_size-1));
    //delete mRoot[1];
    mRoot[1] = mRoot[0];
    mRoot[0] = newBuckets;
    level_item_num[1] = level_item_num[0];
    level_item_num[0] = new_level_item_num;
    level_expand_time++;
    resize_state = 0;
}

template<class T>
bool LevelHash<T>::hasResized() const {
    return resized_started;
}

template<class T>
void LevelHash<T>::setResized(){
    resized_started = false;
}

/*
Function: level_shrink()
Shrink a level hash table in place;
Put a new level at the bottom of the old hash table and only rehash the
items in the top level of the old hash table;
*/
//Not touching this since we are not going to be needing it for now
//COMMENTED OUT TO ENSURE THIS WILL COMPILE
    template<class T>
void LevelHash<T>::level_shrink()
{
    /*
    // The shrinking is performed only when the hash table has very few items.
    if(level_item_num[0] + level_item_num[1] > total_capacity*ASSOC_NUM*0.4){
    printf("The shrinking fails: 2\n");
    exit(1);
    }
    resize_state = 2;
    level_size --;
    level_bucket *newBuckets = (level_bucket *)alignedmalloc(pow(2, level_size - 1)*sizeof(level_bucket));
    level_bucket *interimBuckets = mRoot[0];
    mRoot[0] = mRoot[1];
    mRoot[1] = newBuckets;
    newBuckets = NULL;
    level_item_num[0] = level_item_num[1];
    level_item_num[1] = 0;
    addr_capacity = pow(2, level_size);
    total_capacity = pow(2, level_size) + pow(2, level_size - 1);
    uint64_t old_idx, i;
    for (old_idx = 0; old_idx < pow(2, level_size+1); old_idx ++) {
    for(i = 0; i < ASSOC_NUM; i ++){
    if (interimBuckets[old_idx].token[i] == 1)
    {
    if(level_insert(level, interimBuckets[old_idx].slot[i].key, interimBuckets[old_idx].slot[i].value)){
    printf("The shrinking fails: 3\n");
    exit(1);   
    }
    interimBuckets[old_idx].token[i] = 0;
    }
    }
    } 
    free(interimBuckets);
    level_expand_time = 0;
    resize_state = 0;
    */
}
/*
Function: level_dynamic_query() 
Lookup a key-value item in level hash table via danamic search scheme;
First search the level with more items;
*/
//COmmenting for now since we are not using and will not updatete until we need it
//template <class T>
//uint8_t* LevelHash<T>::level_dynamic_query(BitString key)
//{
/*
   uint64_t f_hash = F_HASH(level, key);
   uint64_t s_hash = S_HASH(level, key);
   uint64_t i, j, f_idx, s_idx;
   if(level_item_num[0] > level_item_num[1]){
   f_idx = F_IDX(f_hash, addr_capacity);
   s_idx = S_IDX(s_hash, addr_capacity); 
   for(i = 0; i < 2; i ++){
   for(j = 0; j < ASSOC_NUM; j ++){
   if (mRoot[i][f_idx].token[j] == 1&&strcmp((char*)mRoot[i][f_idx].slot[j].key, (char*)key) == 0)
   {
   return mRoot[i][f_idx].slot[j].value;
   }
   }
   for(j = 0; j < ASSOC_NUM; j ++){
   if (mRoot[i][s_idx].token[j] == 1&&strcmp((char*)mRoot[i][s_idx].slot[j].key, (char*)key) == 0)
   {
   return mRoot[i][s_idx].slot[j].value;
   }
   }
   f_idx = F_IDX(f_hash, addr_capacity / 2);
   s_idx = S_IDX(s_hash, addr_capacity / 2);
   }
   }
   else{
   f_idx = F_IDX(f_hash, addr_capacity/2);
   s_idx = S_IDX(s_hash, addr_capacity/2);
   for(i = 2; i > 0; i --){
   for(j = 0; j < ASSOC_NUM; j ++){
   if (mRoot[i-1][f_idx].token[j] == 1&&strcmp((char*)mRoot[i-1][f_idx].slot[j].key, (char*)key) == 0)
   {
   return mRoot[i-1][f_idx].slot[j].value;
   }
   }
   for(j = 0; j < ASSOC_NUM; j ++){
   if (mRoot[i-1][s_idx].token[j] == 1&&strcmp((char*)mRoot[i-1][s_idx].slot[j].key, (char*)key) == 0)
   {
   return mRoot[i-1][s_idx].slot[j].value;
   }
   }
   f_idx = F_IDX(f_hash, addr_capacity);
   s_idx = S_IDX(s_hash, addr_capacity);
   }
   }
   return NULL;
   */
//Leaving this so we have a return value for compiler
//return NULL;
//}
/*

Function: level_static_query() 
Lookup a key-value item in level hash table via static search scheme;
Always first search the top level and then search the bottom level;
*/
    template <class T>
T LevelHash<T>::level_static_query(BitString key)
{
    uint64_t f_hash = F_HASH(key);
    uint64_t s_hash = S_HASH(key);
    uint64_t f_idx = F_IDX(f_hash, addr_capacity);
    uint64_t s_idx = S_IDX(s_hash, addr_capacity);
    uint64_t i, j;
    int ASSOC_NUM = mRoot[0][0].getASSOC();
    BitString bad_val = BitString("1");
    for(i = 0; i < 2; i ++){
        for(j = 0; j < ASSOC_NUM; j ++){
            //This should be fixed, leaving prev version in case i fucked something up 
            //if (mRoot[i][f_idx].getToken(j) == 1&&strcmp((char*)mRoot[i][f_idx].getSlot(j).getKey(), (char*)key) == 0)
            // checks if slot is not null first..
            if (mRoot[i][f_idx].getSlot(j)) {
                 BitString temp_key = mRoot[i][f_idx].getSlot(j)->getKey(key);
                if (temp_key == bad_val) {
                    return 0;
                }
                if (mRoot[i][f_idx].getToken(j) == 1 && temp_key == key)
                {
                    return mRoot[i][f_idx].getSlot(j)->getValue();
                }
            }
        }
        for(j = 0; j < ASSOC_NUM; j ++){
            //This should be fixed, leaving prev version in case i fucked something up 
            //if (mRoot[i][s_idx].getToken(j) == 1&&strcmp((char*)mRoot[i][s_idx].getSlot(j).getKey(), (char*)key) == 0)
            if (mRoot[i][s_idx].getSlot(j)) {
                BitString temp_key = mRoot[i][s_idx].getSlot(j)->getKey(key);
                if (temp_key == bad_val) {
                    return 0;
                }
                if (mRoot[i][s_idx].getToken(j) == 1&& temp_key == key)
                {
                    return mRoot[i][s_idx].getSlot(j)->getValue();
                }   
            }
        }
        f_idx = F_IDX(f_hash, addr_capacity / 2);
        s_idx = S_IDX(s_hash, addr_capacity / 2);
    }
    return NULL;
}
/*
Function: level_delete() 
Remove a key-value item from level hash table;
The function can be optimized by using the dynamic search scheme
*/
//Commented out and not updated for now since we are likely not using for now 
    template<class T>
uint8_t LevelHash<T>::level_delete(BitString key)
{
    /*
       uint64_t f_hash = F_HASH(level, key);
       uint64_t s_hash = S_HASH(level, key);
       uint64_t f_idx = F_IDX(f_hash, addr_capacity);
       uint64_t s_idx = S_IDX(s_hash, addr_capacity);

       uint64_t i, j;
       for(i = 0; i < 2; i ++){
       for(j = 0; j < ASSOC_NUM; j ++){
       if (mRoot[i][f_idx].token[j] == 1&&strcmp((char*)mRoot[i][f_idx].slot[j].key, (char*)key) == 0)
       {
       mRoot[i][f_idx].token[j] = 0;
       level_item_num[i] --;
       return 0;
       }
       }
       for(j = 0; j < ASSOC_NUM; j ++){
       if (mRoot[i][s_idx].token[j] == 1&&strcmp((char*)mRoot[i][s_idx].slot[j].key, (char*)key) == 0)
       {
       mRoot[i][s_idx].token[j] = 0;
       level_item_num[i] --;
       return 0;
       }
       }
       f_idx = F_IDX(f_hash, addr_capacity / 2);
       s_idx = S_IDX(s_hash, addr_capacity / 2);
       }
       return 1;
       */
    //filler for compiler
    return 1;
}
/*
Function: level_update() 
Update the value of a key-value item in level hash table;
The function can be optimized by using the dynamic search scheme
*/
    template<class T>
uint8_t LevelHash<T>::level_update(BitString key, T new_value)
{
    uint64_t f_hash = F_HASH(key);
    uint64_t s_hash = S_HASH(key);
    uint64_t f_idx = F_IDX(f_hash, addr_capacity);
    uint64_t s_idx = S_IDX(s_hash, addr_capacity);
    int ASSOC_NUM = mRoot[0][0].getASSOC();
    uint64_t i, j;
    for(i = 0; i < 2; i ++){
        for(j = 0; j < ASSOC_NUM; j ++){
            if (mRoot[i][f_idx].getToken(j) == 1 && mRoot[i][f_idx].getSlot(j)->getKey() ==  key)
            {
                mRoot[i][f_idx].getSlot(j)->setValue(new_value);
                return 0;
            }
        }
        for(j = 0; j < ASSOC_NUM; j ++){
            if (mRoot[i][s_idx].getToken(j) == 1 && mRoot[i][s_idx].getSlot(j)->getKey() ==  key)
            {
                mRoot[i][s_idx].getSlot(j)->setValue(new_value);
                return 0;
            }
        }
        f_idx = F_IDX(f_hash, addr_capacity / 2);
        s_idx = S_IDX(s_hash, addr_capacity / 2);
    }
    return 1;
}
/*
Function: level_insert() 
Insert a key-value item into level hash table;
*/
    template <class T>
uint8_t LevelHash<T>::level_insert(BitString key, T value, int value_size)
{
    uint64_t f_hash = F_HASH(key);
    uint64_t s_hash = S_HASH(key);
    uint64_t f_idx = F_IDX(f_hash,addr_capacity);
    uint64_t s_idx = S_IDX(s_hash,addr_capacity);
    uint64_t i, j;
    int empty_location;
    //create a storage pointer for the new entry that is going to be inserted
    //uint8_t * storage_ptr = (uint8_t *) alignedmalloc(mtu_size * hist_len);
    int ASSOC_NUM = mRoot[0][0].getASSOC();
    //iterate over levels, want to insert in the top level first
    for(i = 0; i < 2; i++){
        //then within the level
        for(j = 0; j < ASSOC_NUM; j++){        
            /*  The new item is inserted into the less-loaded bucket between 
                the two hash locations in each level           
                by iterating over slots and then choosing a bucket ** 
                */      
            if (mRoot[i][f_idx].getToken(j) == 0)
            {
                LevelHashEntry<T>* newEntry = new LevelHashEntry<T>(key, value, nullptr);
                mRoot[i][f_idx].setSlot(j, newEntry);
                //add the storage ptr creation when first inserting the value, other functions that do no insert will
                mRoot[i][f_idx].setToken(j,1);
                level_item_num[i]++;
                return 0;
            }
            if (mRoot[i][s_idx].getToken(j) == 0) 
            {
                LevelHashEntry<T>* newEntry = new LevelHashEntry<T>(key, value, nullptr);
                mRoot[i][s_idx].setSlot(j, newEntry);
                //add the storage ptr creation when first inserting the value, other functions that do no insert will
                mRoot[i][s_idx].setToken(j,1);
                level_item_num[i]++;
                return 0;
            }
        }
        f_idx = F_IDX(f_hash, addr_capacity / 2);
        s_idx = S_IDX(s_hash, addr_capacity / 2);
    }
    f_idx = F_IDX(f_hash, addr_capacity);
    s_idx = S_IDX(s_hash, addr_capacity);
    for(i = 0; i < 2; i++){
        //NULL instead of storage ptr for now
        if(!try_movement(f_idx, i, key, value, NULL)){
            return 0;
        }
        //NULL instead of storage ptr for now
        if(!try_movement(s_idx, i, key, value, NULL)){
            return 0;
        }
        f_idx = F_IDX(f_hash, addr_capacity/2);
        s_idx = S_IDX(s_hash, addr_capacity/2);        
    }
    if(level_expand_time > 0){
        empty_location = b2t_movement(f_idx);
        if(empty_location != -1){
            LevelHashEntry<T>* entry = new LevelHashEntry<T>(key, value, NULL);
            mRoot[1][f_idx].setSlot(empty_location, entry);
            mRoot[1][f_idx].setToken(empty_location,1);
            level_item_num[1] ++;
            return 0;
        }
        empty_location = b2t_movement(s_idx);
        if(empty_location != -1){
            LevelHashEntry<T>* entry = new LevelHashEntry<T>(key, value, NULL);
            mRoot[1][s_idx].setSlot(empty_location,entry);
            mRoot[1][s_idx].setToken(empty_location,1);
            level_item_num[1] ++;
            return 0;
        }
    }
    return 1;
}
/*
Function: try_movement() 
Try to move an item from the current bucket to its same-level alternative bucket;
*/
//TODO KEEPING STORAGE PTR THIS WAY SINCE PASSING NULL FOR NOW
    template <class T>
uint8_t LevelHash<T>::try_movement(uint64_t idx, uint64_t level_num, BitString key, T value, uint8_t *storage_ptr)
{
    uint64_t i, j, jdx;
    int ASSOC_NUM = mRoot[0][0].getASSOC();
    for(i = 0; i < ASSOC_NUM; i++){
        //uint8_t *m_key = mRoot[level_num][idx].slot[i].key;
        //uint8_t *m_value = mRoot[level_num][idx].slot[i].value;
        //uint8_t *m_storage_ptr = mRoot[level_num][idx].slot[i].storage_ptr;
        LevelHashEntry<T> m_entry = *mRoot[level_num][idx].getSlot(i);
        uint64_t f_hash = F_HASH(m_entry.getKey());
        uint64_t s_hash = S_HASH(m_entry.getKey());
        uint64_t f_idx = F_IDX(f_hash, addr_capacity/(1+level_num));
        uint64_t s_idx = S_IDX(s_hash, addr_capacity/(1+level_num));
        if(f_idx == idx)
            jdx = s_idx;
        else
            jdx = f_idx;
        for(j = 0; j < ASSOC_NUM; j ++){
            if (mRoot[level_num][jdx].getToken(j) == 0)
            {
                LevelHashEntry<T>* entry = new LevelHashEntry<T>(m_entry);
                mRoot[level_num][jdx].setSlot(j, entry);
                mRoot[level_num][jdx].setToken(j,1);
                mRoot[level_num][idx].setToken(i,0);
                // The movement is finished and then the new item is inserted
                LevelHashEntry<T>* newEntry = new LevelHashEntry<T>(key, value, storage_ptr);
                mRoot[level_num][idx].setSlot(i, newEntry);
                mRoot[level_num][idx].setToken(i,1);
                level_item_num[level_num]++;
                return 0;
            }
        }       
    }
    return 1;
}
/*
Function: b2t_movement() 
Try to move a bottom-level item to its top-level alternative mRoot;
*/
    template <class T>
int LevelHash<T>::b2t_movement(uint64_t idx)
{
    uint8_t *key, *value, *storage_ptr;
    uint64_t s_hash, f_hash;
    uint64_t s_idx, f_idx;
    uint64_t i, j;
    int ASSOC_NUM = mRoot[0][0].getASSOC();
    for(i = 0; i < ASSOC_NUM; i ++){
        LevelHashEntry<T> entry = *mRoot[1][idx].getSlot(i);
        f_hash = F_HASH(entry.getKey());
        s_hash = S_HASH(entry.getKey());  
        f_idx = F_IDX(f_hash, addr_capacity);
        s_idx = S_IDX(s_hash, addr_capacity);
        for(j = 0; j < ASSOC_NUM; j ++){
            if (mRoot[0][f_idx].getToken(j) == 0) {
                LevelHashEntry<T>* newEntry = new LevelHashEntry<T>(entry);
                mRoot[0][f_idx].setSlot(j, newEntry);
                mRoot[0][f_idx].setToken(j,1);
                mRoot[1][idx].setToken(i,0);
                level_item_num[0]++;
                level_item_num[1]--;
                return i;
            }
            else if (mRoot[0][s_idx].getToken(j) == 0)
            {
                LevelHashEntry<T>* newEntry = new LevelHashEntry<T>(entry);
                mRoot[0][s_idx].setSlot(j, newEntry);
                mRoot[0][s_idx].setToken(j,1);
                mRoot[1][idx].setToken(i,0);
                level_item_num[0]++;
                level_item_num[1]--;
                return i;
            }
        }
    }
    return -1;
}

//==========================================================
// START GETTERS
//==========================================================

//Getter for f_seed
template <class T>
uint64_t LevelHash<T>::getSeed1() const {
    return f_seed;
}

//Getter for s_seed
template <class T>
uint64_t LevelHash<T>::getSeed2() const {
    return s_seed;
}


// //Getter for mRoot level of a table
// //level is either 0 for top level, or 1 for the bottom level
// template <class T>
// LevelHashBucket<T>[] getMRoot(int level) const {
//     return mRoot[level];
// }

//Getter for the size of the top level of the table
template <class T>
uint64_t LevelHash<T>::getSize() const {
    return addr_capacity;
}

//==========================================================
// END GETTERS
//==========================================================

#endif /* defined(__Data_Structure__LevelHash__) */
