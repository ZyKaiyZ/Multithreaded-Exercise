#include <iostream>
#include <string> 
#include <vector> 
#include <utility> 
#include <fstream> 
#include <map>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h> 
#include <cmath>
#include <iomanip>
#include <time.h>
using namespace std; 
typedef pair<string, string> document; //<ID,data> 
typedef pair<string, vector<int>> dataVector; //ID和詞頻向量<ID,termFrequencyVector>
vector<dataVector> termFrequencyVectorList;//ID和詞頻向量的List
double maxAvgCos=0;//最大avgCos
string keyDocID="No KeyDoc!!!";//關鍵文件名稱
vector<document> transferData(vector<string> dataList) { 
	vector<document> temp;  
	for (vector<string>::iterator iter = dataList.begin();iter != dataList.end();iter += 2) {
		temp.push_back({ *iter, *(iter + 1) });
	}
	return temp;
}
void clearPunctuation(string& str) { //標點符號變空白
	for (int i = 0;i < str.length();i++) {
		if (str[i] < '0' ||
			str[i] > '9' && str[i] < 'A' ||
			str[i] > 'Z' && str[i] < 'a' ||
			str[i] > 'z') { //沒有標點符號(非英文和數字)
			str[i] = ' ';
		}
	}
}
bool onlyAlphabet(string str) {
	for (int i = 0;i < str.length();i++) {
		if ('0' <= str[i] && str[i] <= '9') {//有數字
			return false;
		}
	}
	return true;
}
map<string, int> calculateTermFrequency(string str) {//計算詞頻
	map<string, int> temp;
	clearPunctuation(str);
	str += ' ';//讓最後一個字能夠存進list
	string vocabulary;
	for (int i = 0;i < str.length();i++) {
		if (str[i] != ' ') {
			vocabulary += str[i];
		}
		else {//遇到空白
			if (onlyAlphabet(vocabulary) && vocabulary != "") {//純字母
				temp[vocabulary]++;
			}
			vocabulary = "";
		}
	}
	return temp;
}
vector<string> makeVocabularyList(vector<document> documentList) {//儲存所有單字
	map<string, int> termList,temp;
	for (auto iter : documentList) {//搜尋整份文件
		temp = calculateTermFrequency(iter.second);
		termList.insert(temp.begin(), temp.end());
	}
	vector<string> vocabularyList;
	for (auto iter2 :termList) {
		vocabularyList.push_back(iter2.first);
	}
	return vocabularyList;
}
vector<dataVector> makeTermFrequencyVector(vector<document> documentList) {
	vector<dataVector> temp;
	vector<string> vocabularyList = makeVocabularyList(documentList);//所有單字
	for (auto iter : documentList) {
		map<string, int> termFrequencyList = calculateTermFrequency(iter.second);
		vector<int> termFrequency;
		for (auto iter2 : vocabularyList) {//得到詞頻向量
			termFrequency.push_back(termFrequencyList[iter2]);
		}
		temp.push_back({ iter.first,termFrequency });
	}
	return temp;
}
void printVector(vector<int> tmp){
	cout<<"[";
	for(vector<int>::iterator iter=tmp.begin();iter!=tmp.end()-1;iter++){
		cout<<*iter<<",";
	}
	cout<<tmp.back();
	cout<<"]\n";
}
double getMold(vector<int> vec){//求向量的長度
        int num = vec.size();
        double sum = 0;
        for (int i = 0; i<num; i++){
            sum += vec[i] * vec[i];
		}
        return sqrt(sum);
}
double calculateVectorCosine(vector<int> docVector1,vector<int> docVector2){
	int dotProduct=0;
	for(int i=0;i<docVector1.size();i++){
		dotProduct+=docVector1[i]*docVector2[i];//點積
	}
	double doc1Sum=getMold(docVector1),doc2Sum=getMold(docVector2);
	if(doc1Sum==0||doc2Sum==0){
		return 0;
	}
	else{
		return dotProduct/(doc1Sum*doc2Sum);
	}
}
void* calculateAvgCosine(void* arg){
	pair<timespec,timespec> CPUthreadsTime;//<startTime,endTime>
	clock_gettime(CLOCK_THREAD_CPUTIME_ID,&CPUthreadsTime.first);//開始時間
	string* threadName=(string*) arg;
	cout<<"[TID:"<<pthread_self()<<"] DocID:"<<*threadName<<" ";
	vector<dataVector> tmp;
	dataVector currentData;
	for(auto iter:termFrequencyVectorList){
		if(iter.first==*threadName){//相同ID
			currentData=iter;
			printVector(iter.second);
		}
		else{
			tmp.push_back(iter);
		}
	}
	double cosSum=0;
	for(auto iter:tmp){
		cout<<"[TID:"<<pthread_self()<<"] cosine("<<*threadName<<","<<iter.first<<")=";
		double cosNum=calculateVectorCosine(currentData.second,iter.second);
		cout<<setprecision(4)<<cosNum<<"\n";
		cosSum+=cosNum;
	}
	double avgCos=cosSum/termFrequencyVectorList.size();
	if(maxAvgCos<avgCos){
		maxAvgCos=avgCos;
		keyDocID=currentData.first;
	}
	cout<<"[TID:"<<pthread_self()<<"] Avg_cosine: "<<avgCos<<"\n";
	clock_gettime(CLOCK_THREAD_CPUTIME_ID,&CPUthreadsTime.second);//結束時間
	cout<<"[TID:"<<pthread_self()<<"] CPU time: "<<double(CPUthreadsTime.second.tv_nsec-CPUthreadsTime.first.tv_nsec)/CLOCKS_PER_SEC<<"ms\n==========================================================\n";
	pthread_exit(0);
}
int main(int argc, char* argv[]) {
	pair<timespec,timespec> CPUtime;//<startTime,endTime>
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&CPUtime.first);//開始時間
	fstream fin; 
	string data; 
	vector<string> dataList;
	fin.open(argv[1]);//開啟檔案 
	while (getline(fin, data) && data != "") {
		dataList.push_back(data);
	} 
	vector<document> documentList = transferData(dataList);//整理資料 
 	termFrequencyVectorList = makeTermFrequencyVector(documentList);
	int threadNum=termFrequencyVectorList.size();
	pthread_t threads[threadNum];
	int i=0;
	for(auto iter:termFrequencyVectorList){
		string threadName=iter.first;
		pthread_create(&threads[i],NULL,calculateAvgCosine,(void*)&threadName);
		cout<<"[Main thread]: create TID:"<<threads[i]<<",Doc: "<<threadName<<"\n";
		sleep(0.5);
		i++;
	}
 	for(int j=0;j<threadNum;j++){
		pthread_join(threads[j],NULL);
	}
	cout<<"[Main thread] KeyDocID: "<<keyDocID<<" Highest Average Cosine: "<<maxAvgCos<<"\n"; 
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&CPUtime.second);//結束時間
	cout<<"[Main thread] CPU time: "<<double(CPUtime.second.tv_nsec-CPUtime.first.tv_nsec)/CLOCKS_PER_SEC<<"ms\n";
}


