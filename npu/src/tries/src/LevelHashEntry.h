//
// LevelHashEntry.h
//
#ifndef __Data_Structure__LevelHashEntry__
#define __Data_Structure__LevelHashEntry__
#include <functional>
#include "Trie.h"
#include "systemc.h"
#include "tlmsingletoninclude.h"
template <class T>
class LevelHashEntry {
    public:
        // Constructor
        LevelHashEntry(BitString iKey, T iVal, uint8_t* iStoragePtr);
        LevelHashEntry();
        LevelHashEntry(const LevelHashEntry &iCopy);
        // Destructor
        ~LevelHashEntry();
        // Getters
        BitString getKey() const;
        T getValue() const;
        uint8_t* getStoragePtr() const;
        // Setters
        void setKey(BitString iKey);
        void setValue(T iVal, int iSize);
        void setStoragePtr(uint8_t* storagePtr);
        void* operator new(long unsigned) throw(const char*);
        void* operator new[](long unsigned) throw(const char*);
        void operator delete(void*);
        void operator delete[](void*);
    private:
        BitString mKey;
        T mValue;
        uint8_t* mStoragePtr;
        int tlm_addr;
        // also don't eventually need this..
        // Our changes below here ============
        int mVal_PtrSize;
        // TODO come back to this
        //
        // Wants to map the hash given by the accel to an address to use 
        // in the rest of the workflow
        std::unordered_map<unsigned int, int> hashToAddr;
};


//
//  LevelHashEntry.cpp
//  Level Hash Entry DS
//
//#include "LevelHashEntry.h"
/// ==========================
//
//  Constructors
//
/// ==========================
//iKey is the hashed version of the key
template <class T>
LevelHashEntry<T>::LevelHashEntry(BitString iKey, T iVal, uint8_t* iStoragePtr) :
    mKey(iKey), //0
    mValue(iVal), //1
    mStoragePtr(iStoragePtr) //3
{
    // the hashToAdr map should have a reference from iKey to addr 
    // TODO - come back to this
    int addr = tlmsingelton::getInstance().tlmvarptr->allocate_mem(32*5); //Allocate and Construct in tlm memory
    this->tlm_addr = addr; // wont work
    std::size_t val = 0;
    //actually writing the key
    //tlmsingelton::getInstance().tlmvarptr->write_mem(static_cast<std::size_t>(iKey),this->tlm_addr);
    tlmsingelton::getInstance().tlmvarptr->write_mem(iKey.toInt(),this->tlm_addr);
    //filler zero values
    //addresssing is based on the virtual address given by the memory controller
    tlmsingelton::getInstance().tlmvarptr->write_mem(static_cast<std::size_t>(val),this->tlm_addr+1); //val
    tlmsingelton::getInstance().tlmvarptr->write_mem(static_cast<std::size_t>(val),this->tlm_addr+2);
}
template <class T>
LevelHashEntry<T>::LevelHashEntry(){
        this->mKey = BitString();
        this->mStoragePtr = nullptr;
        this->mValue = 0; 
        // the hashToAdr map should have a reference from iKey to addr 
        // TODO - come back to this
        int addr = tlmsingelton::getInstance().tlmvarptr->allocate_mem(32*5); //Allocate and Construct in tlm memory
        this->tlm_addr = addr; // wont work
        std::size_t val = 0;
        tlmsingelton::getInstance().tlmvarptr->write_mem(static_cast<std::size_t>(val),this->tlm_addr);
        tlmsingelton::getInstance().tlmvarptr->write_mem(static_cast<std::size_t>(val),this->tlm_addr+1);
        tlmsingelton::getInstance().tlmvarptr->write_mem(static_cast<std::size_t>(val),this->tlm_addr+2);
    }
/// ==========================
//
//  Copy Constructor
//
/// ==========================
template <class T>
LevelHashEntry<T>::LevelHashEntry(const LevelHashEntry<T>& iCopy) : 
    mKey(iCopy.getKey()), mValue(iCopy.getValue()), mStoragePtr(iCopy.getStoragePtr()) {}

/// ==========================
//
//  Destructor
//
/// ==========================
template <class T>
LevelHashEntry<T>::~LevelHashEntry() {
    /*    if (mEntryTypeFlag == 0 && mPtr != 0) {
          delete [] mPtr;
          mPtr = 0;
          }
          */
}
/// ==========================
//
//  Getters
//
/// ==========================
template <class T>
BitString LevelHashEntry<T>::getKey() const {
    std::size_t val =   tlmsingelton::getInstance().tlmvarptr->read_mem(this->tlm_addr);
    // let's first return mKey to verify correctness before actually getting from memory
    return mKey;
    //return static_cast<unsigned int>(val);
}
template <class T>
T LevelHashEntry<T>::getValue() const {
    std::size_t val = tlmsingelton::getInstance().tlmvarptr->read_mem(this->tlm_addr+1);
    return mValue;
}
template <class T>
uint8_t* LevelHashEntry<T>::getStoragePtr() const {
    std::size_t val = tlmsingelton::getInstance().tlmvarptr->read_mem(this->tlm_addr+2);
    return mStoragePtr;
}
/// ==========================
//
//  Setters
//
/// ==========================
template <class T>
void LevelHashEntry<T>::setKey(BitString iKey) {
    mKey = iKey;
    tlmsingelton::getInstance().tlmvarptr->allocate(iKey.toInt(),this->tlm_addr);
}
template <class T>
void LevelHashEntry<T>::setValue(T iVal, int iSize) {
    mValue = iVal;
    mVal_PtrSize = iSize;
    tlmsingelton::getInstance().tlmvarptr->allocate(0,this->tlm_addr+1);
}
template <class T>
void LevelHashEntry<T>::setStoragePtr(uint8_t* storagePtr) {
    mStoragePtr = storagePtr;
    tlmsingelton::getInstance().tlmvarptr->allocate(0,this->tlm_addr+2);
}
template <class T>
void* LevelHashEntry<T>::operator new(long unsigned size) throw(const char*) {
#if debug_node_new
    cout << "   Node::operator new(" << size << ")\t";
#endif
    void *p = malloc(size); if (!p) throw "Node::operator new() error";
#if debug_node_new
    cout << static_cast<void*>(p) << endl;
#endif
    return p;
}
template <class T>
void* LevelHashEntry<T>::operator new[](long unsigned size) throw(const char*) {
    // cout << "   Node::operator new[](" << size << ")\t";
    void *p = malloc(size); if (!p) throw "Node::operator new[]() error";
    // cout << static_cast<void*>(p) << endl;
    return p;
}
template <class T>
void LevelHashEntry<T>::operator delete(void *p) {
}
template <class T>
void LevelHashEntry<T>::operator delete[](void *p) {
}

#endif /* defined(__Trie_Data_Structure__HashTableEntry__) */
