#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "the3.h"

/*
    Create a cache by reading inputs from stdin
*/
DomainFreqNode* makefreqnode(char *domain,char *inp_freq){
    DomainFreqNode* ptrfreq;
    ptrfreq = (DomainFreqNode *) malloc(sizeof(DomainFreqNode));
    if (ptrfreq)
    {
        ptrfreq -> name = (char *) malloc(sizeof(char)*1000);
        strcpy(ptrfreq -> name,domain);
        ptrfreq -> freq = atoi(inp_freq);
    }
    ptrfreq -> next = NULL;
    ptrfreq -> prev = NULL;
    return ptrfreq;
}
DomainFreqNode* insert_domain(DomainFreqNode **list,char *domain,char *inp_freq){
    DomainFreqNode *tmp;
    tmp = makefreqnode(domain,inp_freq);
    if (tmp)
    {
        
        int swapped = 1;
        DomainFreqNode *curr,*prevptr,*iterator;
        curr = *list;  prevptr = NULL;
        while (curr && (atoi(inp_freq) <= curr->freq))
        {
                prevptr = curr;
                curr = curr->next;
        }
        tmp->next = curr;
        if (curr)
            curr->prev = tmp;
        if (prevptr)
            {
                prevptr->next = tmp;
                tmp->prev = prevptr;
            }
        else
        {
            *list = tmp;
        }

        
        while (swapped)
        {
            iterator = *list;
            swapped = 0;
            while (iterator && iterator->next)
            {
                if ((strcmp(iterator->name,iterator->next->name) > 0) && (iterator->freq == iterator->next->freq))
                {
                    char *str,*str2;
                    swapped = 1;
                    str = (char *) malloc(sizeof(char)*1000);
                    str2 = (char *) malloc(sizeof(char)*1000);
                    strcpy(str,iterator->name);
                    strcpy(str2,iterator->next->name);
                    strcpy(iterator->name,str2);
                    strcpy(iterator->next->name,str);
                    free(str);
                    free(str2);
                }    
                iterator = iterator->next;    
            }
        }
    }
    return *list;
}
Cache* createCache()
{
    DomainFreqNode *beginnerptr = NULL;
    Cache* mycache;
    CacheNode *mycachenode;
    DomainFreqList *myfreqlist;
    int i;
    char *input; 
    mycache = (Cache *) malloc(sizeof(Cache));  
    scanf("%d %d\n",&(mycache->cacheLimit),&(mycache->mediaCount));
    mycachenode = (CacheNode *) malloc(sizeof(CacheNode)*(mycache->mediaCount));
    myfreqlist = (DomainFreqList *) malloc(sizeof(DomainFreqList)*(mycache->mediaCount));
    input = (char *) malloc(sizeof(char)*1000); 
    mycache->head = mycachenode;
    mycache->tail = mycachenode + mycache->mediaCount - 1;
    mycachenode[0].prev = NULL;
    mycache->currentSize = 0;
    for (i=0;i<mycache->mediaCount;i++){ 
        char *domname_inp,*freq_inp;
        char *name_inp_helper;
        char *freq_inp_helper;
        fgets(input,1000,stdin);         
        mycachenode[i].media.name = (char *) malloc(sizeof(char)*1000);
        strcpy(mycachenode[i].media.name,strtok(input," "));
        mycachenode[i].media.size = atoi(strtok(NULL," "));
        if (i + 1 == mycache->mediaCount)
            mycachenode[i].next = NULL;
        else
            mycachenode[i].next = mycachenode + i + 1;
        if (i >= 1)
            mycachenode[i].prev = mycachenode + i - 1;
        mycachenode[i].domainFreqList = myfreqlist + i;
        domname_inp = (char *) malloc(sizeof(char)*1000);
        freq_inp = (char *) malloc(sizeof(char)*1000);
        while (1)
        {
            name_inp_helper = strtok(NULL," ");
            if (name_inp_helper)
                freq_inp_helper = strtok(NULL," ");
            if (name_inp_helper && freq_inp_helper)
                {strcpy(domname_inp,name_inp_helper); strcpy(freq_inp,freq_inp_helper);}
            else
                break;
            insert_domain(&(beginnerptr),domname_inp,freq_inp);
        }
        free(domname_inp);
        free(freq_inp);
        mycachenode[i].domainFreqList->head = beginnerptr;
        while (beginnerptr)
            {
                mycachenode[i].domainFreqList->tail = beginnerptr;
                beginnerptr = beginnerptr->next;
            }
        beginnerptr = NULL;
        mycache->currentSize += mycachenode[i].media.size;
        }   
    free(input);
    return mycache;
}

/*
    Prints cache information and its contents
*/
void printCache(Cache* cache)
{
    if (cache->currentSize == 0)
        printf("The Cache is Empty\n");
    else{
    int i;
    CacheNode* tmp;
    DomainFreqNode* tmpfreq;
    tmp = cache->head;
    tmpfreq = cache->head->domainFreqList->head;
    printf("-------------- Cache Information --------------\n");
    printf("Cache Limit: %d KB\n",cache->cacheLimit);
    printf("Current Size: %d KB\n",cache->currentSize);
    printf("Media Count: %d\n",cache->mediaCount);
    printf("Cache Media Info:\n");
    for (i = 0;i<cache->mediaCount;i++){
        printf("    Name: %s    Size: %d KB\n",tmp->media.name,tmp->media.size);
        printf("    ------- Media Domain Frequencies -------\n");
        while(tmpfreq){
            printf("        Name: %s    Freq: %d\n",tmpfreq->name,tmpfreq->freq);
            tmpfreq = tmpfreq->next;
        }
        tmp = tmp->next;
        if (tmp)
            tmpfreq = tmp->domainFreqList->head;
        printf("    ----------------------------------------\n");
    }
    printf("----------------------------------------------\n");}

    
}


/*
    Adds media to the cache
*/
CacheNode* addMediaRequest(Cache* cache, Media media, char* domain)
{
    CacheNode* tmp = findMedia(cache,media.name);
    if (tmp)
    {
        DomainFreqNode* tmp_freq = tmp->domainFreqList->head;
        while (tmp_freq)
        {
            if (strcmp(tmp_freq->name,domain) == 0)
                break;
            tmp_freq = tmp_freq->next;
        }
        if (tmp_freq)
        {
            if (tmp_freq == tmp->domainFreqList->head)
                tmp_freq->freq += 1;
            if (tmp_freq == tmp->domainFreqList->tail)
            {
                char str[10];
                tmp_freq->freq += 1;
                tmp_freq->prev->next = NULL;
                tmp->domainFreqList->tail = tmp_freq->prev;
                sprintf(str,"%d",tmp_freq->freq);
                insert_domain(&(tmp->domainFreqList->head),domain,str);
                if (tmp->domainFreqList->tail->next != NULL)
                    tmp->domainFreqList->tail = tmp->domainFreqList->tail->next;
            }
            else
            {
                char ptr[10];
                tmp_freq->prev->next = tmp_freq->next;
                tmp_freq->next->prev = tmp_freq->prev;
                tmp_freq->freq += 1;
                sprintf(ptr,"%d",tmp_freq->freq);
                insert_domain(&(tmp->domainFreqList->head),domain,ptr);
            }
        }
        else
        {
            insert_domain(&(tmp->domainFreqList->head),domain,"1");
            if (tmp->domainFreqList->tail->next != NULL)
                tmp->domainFreqList->tail = tmp->domainFreqList->tail->next;
        }
        if (tmp == cache->head)
            return tmp;
        if (tmp == cache->tail)
        {
            cache->tail->prev->next = NULL;
            cache->tail = cache->tail->prev;
            tmp->prev = NULL;
            tmp->next = cache->head;
            cache->head->prev = tmp;
            cache->head = tmp;
            return tmp;    
        }
        else
        {
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            tmp->prev = NULL;
            tmp->next = cache->head;
            cache->head->prev = tmp;
            cache->head = tmp;
            return tmp;
        }
        
    }
    else
    {
        if ((cache->cacheLimit - cache->currentSize) >= media.size)
        {
            CacheNode *nodeptr;
            nodeptr = (CacheNode *) malloc(sizeof(CacheNode));
            nodeptr->media.size = media.size;
            nodeptr->media.name = (char *) malloc(sizeof(char)*1000);
            strcpy(nodeptr->media.name,media.name);
            nodeptr->domainFreqList = (DomainFreqList *) malloc(sizeof(DomainFreqList));
            nodeptr->next = cache->head;
            nodeptr->prev = NULL;
            if (cache->head)
                cache->head->prev = nodeptr;
            cache->head = nodeptr;
            nodeptr->domainFreqList->head = makefreqnode(domain,"1");
            nodeptr->domainFreqList->tail = nodeptr->domainFreqList->head;
            cache->currentSize += media.size;
            cache->mediaCount += 1;
            return nodeptr;

        }
        else
        {
            deleteMedia(cache,cache->tail->media.name);
            addMediaRequest(cache,media,domain);
        }
    }
}

/*
    Finds the corresponding media's node in the cache
    Return NULL if it is not found
*/
CacheNode* findMedia(Cache* cache, char* name)
{
    CacheNode* tmp;
    tmp = cache->head;
    while(tmp){
        if (strcmp(tmp->media.name,name) == 0)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

/*
    Finds a media from its name and deletes it's node from the cache
    Do not do anything if media is not found
*/
void deleteMedia(Cache* cache, char* name)
{
    if (findMedia(cache,name) == NULL)
        return;
    else{
        CacheNode* tmp;
        tmp = findMedia(cache,name);
        cache->currentSize = cache->currentSize - tmp->media.size;
        cache->mediaCount -= 1;
        if (tmp == cache->head){
            cache->head = tmp->next;
            if (cache->head)
                cache->head->prev = NULL;
            free(tmp->media.name);
            return;
        }
        if (tmp == cache->tail){
            cache->tail = cache->tail->prev;
            if (cache->tail)
                cache->tail->next = NULL;
            free(tmp->media.name);
            return;
        }
        else{
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            free(tmp->media.name);
        }

    }
}
int main(){
    Cache * cache;
    Media media1;
    CacheNode *deneme;
    cache = createCache();
    media1.name = "video";
    media1.size = 310;
    addMediaRequest(cache, media1, "tr");
    deleteMedia(cache,"video");
    printCache(cache);
}