#ifndef cs32proj4_HashTable_h
#define cs32proj4_HashTable_h

unsigned int computeHash(int key);
unsigned int computeHash(std::string key);

template<typename KeyType, typename ValueType>
class HashTable
{
public:
    HashTable(unsigned int numBuckets, unsigned int capacity);
    ~HashTable();
    bool isFull() const;
    bool set(const KeyType& key, const ValueType& value, bool permanent = false);
    bool get(const KeyType& key, ValueType& value) const;
    bool touch(const KeyType& key);
    bool discard(KeyType& key, ValueType& value);
    
    
private:
    HashTable(const HashTable&);
    HashTable& operator=(const HashTable&);
    
    unsigned int m_numBuckets;
    unsigned int m_capacity;
    unsigned int m_size;          //m_size records the number of nodes in current hash table
    
    //define the node in the open hash table . pointer next and prev point to the neighbouring nodes in the hash table.
    //pointer queueNext and queuePrev point to neighbouring nodes in the queue.
    struct Node
    {
        KeyType key;
        ValueType value;
        Node* next;
        Node* prev;
        Node* queueNext;
        Node* queuePrev;
    };
    
    Node** m_bucket;
    Node* queuefront;  //points to the first node in the queue
    Node* queueback;    //points to the last node in the queue
    
    //some helper function
    void moveToQueueEnd(Node* p);     //move the node p points to to the end of queue
    Node* find(const KeyType& key) const;        //find the key in hash table, return the pointer
    
};

///////////////////hash table implemention////////////////////////////////////
template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::HashTable(unsigned int numBuckets, unsigned int capacity)
{
    m_numBuckets=numBuckets;
    m_capacity=capacity;
    m_size=0;
    //initialize the hashtable buckets
    m_bucket=new Node*[m_numBuckets];
    for (int i=0; i<m_numBuckets; i++) {
        m_bucket[i]=nullptr;
    }
    queuefront=nullptr;
    queueback=nullptr;
    
}


template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::~HashTable()
{
    //delete the nodes if any
    for (int i=0; i<m_numBuckets; i++) {
        while (m_bucket[i]!=nullptr)
        {
            Node* temp=m_bucket[i];
            m_bucket[i]=temp->next;
            delete temp;
        }
    }
    delete[] m_bucket;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::isFull() const
{
    return m_size>=m_capacity;
}


template<typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::set(const KeyType& key, const ValueType& value, bool permanent)
{
    unsigned int bucket=computeHash(key);
    bucket=bucket%m_numBuckets;
    Node *current, *previous=nullptr;
    //find if the insert key is already in the table
    for (current=m_bucket[bucket]; current!=nullptr; current=current->next)
    {
        if (key==current->key)
        {
            if (value==current->value)    //return false if the association is in the table
                return false;
            else                       //if find the key, update the existing association
            {
                current->value=value;
                if (current->queuePrev!=nullptr || current==queuefront) //for non-permanent nodes, move it to FIFO queue end
                {
                    moveToQueueEnd(current);
                }
                return true;
                
            }
        }
        previous=current;
    }
    
    //if not find, insert new node to hash table
    current=new Node;
    current->key=key;
    current->value=value;
    current->next=nullptr;
    current->queueNext = nullptr;
    current->queuePrev = nullptr;
    m_size++;
    
    if (previous!=nullptr)           //if in the middle or back
    {
        previous->next=current;
        current->prev=previous;
    }
    else                     //insert in the front
    {
        m_bucket[bucket]=current;
        current->prev=m_bucket[bucket];
        
    }
    
    
    if (!permanent) {                  //link to queue if not permant
        if (queuefront==nullptr)
        {
            queuefront=current;
            current->queuePrev=nullptr;
        }
        else
        {
            current->queuePrev=queueback;
            queueback->queueNext=current;
        }
        current->queueNext=nullptr;
        queueback=current;
        
    }
    return true;
}


template<typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::get(const KeyType& key, ValueType& value) const
{
    Node *p=find(key);
    if (p!=nullptr) {
        value=p->value;
        return true;
    }
    return false;
}

//if premanent, return false; if not permanent, return true
template<typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::touch(const KeyType& key)
{
    Node *p=find(key);
    if (p==nullptr) {
        return false;
    }
    if (p->queuePrev!=nullptr || p==queuefront) //not permanent, move to end
    {
        moveToQueueEnd(p);
        return true;
    }
    return false;
    
}

template<typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::discard(KeyType& key, ValueType& value)
{
    if (queuefront==nullptr) {      //false if no non-permanent association
        return false;
    }

    key=queuefront->key;          //delete the node from queue
    value=queuefront->value;
    Node* p=queuefront;
    queuefront=queuefront->queueNext;
    
    m_size--;                       //delete the node from hashtable
    if (p->next!=nullptr) {
        p->next->prev=p->prev;
    }
    p->prev->next=p->next;

    delete p;
    return true;
    
}



//////////////////////helper functions////////////////////////
template<typename KeyType, typename ValueType>
void HashTable<KeyType, ValueType>::moveToQueueEnd(Node* p)
{
    if (p==queueback)
    {
        return;
    }
    
    if (p==queuefront)        //delete the node at the front of queue
    {
        queuefront=p->queueNext;
    }
    else                       //delete the node in the middle of the queue
    {
        p->queuePrev->queueNext=p->queueNext;
        p->queueNext->queuePrev=p->queuePrev;
    }

    queueback->queueNext=p;
    p->queuePrev=queueback;
    p->queueNext=nullptr;
    queueback=p;
    return;
}

template<typename KeyType, typename ValueType>
typename HashTable<KeyType, ValueType>::Node* HashTable<KeyType, ValueType>::find(const KeyType& key) const
{
    unsigned int bucket=computeHash(key);
    bucket=bucket%m_numBuckets;
    Node *p;
    for (p=m_bucket[bucket]; p!=nullptr; p=p->next)
    {
        if (key==p->key)
            return p;
    }
    return nullptr;
    
}


#endif
