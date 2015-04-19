#include "hostpool.h"

HostPool::HostPool():mTimeout(10000)
{
}

int HostPool::getSelectedCount()
{
    return mSelectedCount;
}

unsigned int HostPool::size()
{
    QMutexLocker locker(&mMutex);
    return mItemList.size();
}

void HostPool::selectAll()
{
    QMutexLocker locker(&mMutex);
    mSelectedCount = this->mItemList.size();
    unsigned int i;
    for(i=0; i<this->mItemList.size(); ++i)
    {
        this->mItemList.at(i)->checked = true;
    }
}

void HostPool::unselectAll()
{
    QMutexLocker locker(&mMutex);
    mSelectedCount = 0;
    unsigned int i;
    for(i=0; i<this->mItemList.size(); ++i)
    {
        this->mItemList.at(i)->checked = false;
    }
}

void HostPool::reverseSelect()
{
    QMutexLocker locker(&mMutex);
    mSelectedCount = this->mItemList.size() - mSelectedCount;
    unsigned int i;
    for(i=0; i<this->mItemList.size(); ++i)
    {
        this->mItemList.at(i)->checked = !this->mItemList.at(i)->checked;
    }
}

time_t HostPool::getTimeout() const
{
    return mTimeout;
}

void HostPool::setTimeout(const time_t &value)
{
    mTimeout = value;
}


void HostPool::putItem(const HostInfo& hostInfo)
{
    QString address = (hostInfo.addr.toString() + ":" + QString("%1").arg(hostInfo.port));
    QMutexLocker locker(&mMutex);
    //find item in index
    map<QString, HostItem*>::iterator it = mItemIndex.find(address);
    HostItem *pItem = NULL;
    //if it exist, update item
    if(it != mItemIndex.end())
    {
        pItem = it->second;
    }
    else //else push it back and create index in map
    {
        pItem = new HostItem;
        mItemList.push_back(pItem);
        mItemIndex[address] = pItem;
        pItem->checked = false;
    }
    //update access time
    pItem->lastAccessTime = clock();
    pItem->info = hostInfo.info.toString();
    pItem->hostInfo = hostInfo;
    pItem->address = address;
}

void HostPool::cleanTimeoutItem()
{
    time_t expiredTime = clock() - getTimeout();

    QMutexLocker locker(&mMutex);
    vector<HostItem*>::iterator it = mItemList.begin();
    while(it!=mItemList.end())
    {
        HostItem* pItem = (*it);
        if( pItem->lastAccessTime < expiredTime )
        {
            if(pItem->checked)
            {
                -- mSelectedCount;
            }
            map<QString, HostItem*>::iterator mapIt = mItemIndex.find(pItem->address);
            if(mapIt != mItemIndex.end())
            {
                mItemIndex.erase(mapIt);
            }
            mItemList.erase(it);
            delete pItem;
            continue;
        }
        ++it;
    }
}

void HostPool::cleanAll()
{
    QMutexLocker locker(&mMutex);

    mItemList.clear();
    mItemIndex.clear();
}


vector<HostInfo > HostPool::getSelectedHostAddr()
{
    vector<HostInfo > addrList;
    QMutexLocker locker(&mMutex);
    vector<HostItem*>::iterator it = mItemList.begin();
    while(it!=mItemList.end())
    {
        if((*it)->checked)
        {
            addrList.push_back((*it)->hostInfo);
        }
        ++it;
    }
    return addrList;
}

bool HostPool::getHostItem(int index, HostItem& item)
{
    QMutexLocker locker(&mMutex);
    if( index >= (int)this->mItemList.size() ) return false;
    item = *mItemList.at(index);
    return true;
}

void HostPool::checked(int index, bool isChecked)
{
    QMutexLocker locker(&mMutex);
    if( isChecked && !( this->mItemList.at(index)->checked )  )
    {
        ++mSelectedCount;
    } else if( isChecked && ( this->mItemList.at(index)->checked )  )
    {
        --mSelectedCount;
    }
    this->mItemList.at(index)->checked = isChecked;
}
