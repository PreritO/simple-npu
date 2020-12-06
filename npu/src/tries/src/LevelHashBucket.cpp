
// //
// //  LevelHashBucket.cpp
// //  Level Hash Bucket 
// //
// #include "LevelHashBucket.h"
// /// ==========================
// //
// //  Constructors
// //
// /// ==========================
// //Only using an empty defualt constructor (at this point)
// //info will be filled in later
// template <class T>
// LevelHashBucket<T>::LevelHashBucket() : 
// tokens(0), 
// slots(0) {
// //Leaving this empty, all memory allocation should occur in the slots.
// }
// /// ==========================
// //
// //  Copy Constructor
// //
// /// ==========================
// template <class T>
// LevelHashBucket<T>::LevelHashBucket(const LevelHashBucket<T>& iCopy) : 
// {
// //create bucket
//    LevelHashBucket bucket = new LevelHashBucket();
// //copy using getters/setters
// for(int i = 0; i < ASSOC_NUM; i++) {
//        bucket.setToken(i, iCopy.getToken(i));
//        bucket.setSlot(i, iCopy.getSlot(i));
//    } 
// //return
// return bucket;
// }
// /// ==========================
// //
// //  Destructor
// //
// /// ==========================
// template <class T>
// HashTableBucket<T>::~HashTableBucket() {
// /*    if (mEntryTypeFlag == 0 && mPtr != 0) {
//         delete [] mPtr;
//         mPtr = 0;
//     }
// */
// }
// /// ==========================
// //
// //  Getters
// //
// /// ==========================
// //Getter function for the token, takes index, returns the token
// template <class T>
// uint8_t LevelHashBucket<T>::getToken(int index) const {
// //removing the dummy read for now, wont affect stats
// //std::size_t val =   tlmsingelton::getInstance().tlmvarptr->read_mem(this->tlm_addr);
// return tokens[index];
// }
// //getter for the ASSOC_NUM
// template <class T>
// int LevelHashBucket<T>::getASSOC() const {
// return ASSOC_NUM;
// }
// //getter for level hashbucket, takes slot number
// template <class T>
// LevelHashEntry LevelHashBucket<T>::getSlot(int index) const {
// //removing this for now since we only want one penalty that happens in slot access
// //std::size_t val = tlmsingelton::getInstance().tlmvarptr->read_mem(this->tlm_addr+1);
// return slots[index];
// }
// /// ==========================
// //
// //  Setters
// //
// /// ==========================
// template <class T>
// void LevelHashBucket<T>::setToken(int index, uint8_t value) {
//     tokens[index] = value;
// //dont care about penalty for now
// //tlmsingelton::getInstance().tlmvarptr->allocate(static_cast<std::size_t>(iKey),this->tlm_addr);
// }
// template <class T>
// void LevelHashBucket<T>::setSlot(int index, LevelHashEntry bucket) {
// //dont care about penalty
// //tlmsingelton::getInstance().tlmvarptr->allocate(0,this->tlm_addr+1);
//     slots[index] = bucket;
// }
// template <class T>
// void* LevelHashBucket<T>::operator new(long unsigned size) throw(const char*) {
// #if debug_node_new
//     cout << "   Node::operator new(" << size << ")\t";
// #endif
// void *p = malloc(size); if (!p) throw "Node::operator new() error";
// #if debug_node_new
//     cout << static_cast<void*>(p) << endl;
// #endif
// return p;
// }
// template <class T>
// void* LevelHashBucket<T>::operator new[](long unsigned size) throw(const char*) {
// // cout << "   Node::operator new[](" << size << ")\t";
// void *p = malloc(size); if (!p) throw "Node::operator new[]() error";
// // cout << static_cast<void*>(p) << endl;
// return p;
// }
// template <class T>
// void LevelHashBucket<T>::operator delete(void *p) {
// }
// template <class T>
// void LevelHashBucket<T>::operator delete[](void *p) {
// }