# Multithreaded-Programming-Exercise
## 利用Multi-threading來同時計算餘弦相似係數(Cosine similarity coefficient)
### 目的說明:
在多份文件中尋找關鍵文件，所謂關鍵文件說明如下，在一個有M個文件的文件集合 $D={d_1,d_2 , ... , d_M}$ 中，關鍵文件$d_k$ 就是與其他文件的相似度平均值最高的文件。
1. 每一份文件對其他 $M-1$ 份文件的平均餘弦相似係數 $(Avg$ _ $Cos(d_1, d_2))$ 用一個單獨的thread來計算出來。
2. 如果文章中有標點符號，這些標點符號都先轉成空白字元。
3. 只考慮純字母組成的詞，其他如果不是純字母組成的詞則予以忽略不計。例如: "64"、"test20"。
5. 若找不到關鍵文件則輸出"No KeyDoc!!!"
6. 如果有多個文件具有相同的平均餘弦相似係數，則由它們的文件ID大小來決定。ID最小的為關鍵文件。
### 餘弦相似係數(Cosine similarity coefficient)計算方式:
1. 先算出每一篇文章中的詞在文章中出現的數量，這個數量稱之為詞頻（term frequency）。
2. 按照所有文章的全部詞彙將每一篇文章轉換成一個詞頻向量。
3. 接著算出來源文章向量（Vs）與其他每一篇文章向量（Vx）的向量cosine 值。這個公式如下：
    $$Sim(V_s,V_x)=cos(V_s,V_x)=\frac{V_s\cdot V_x}{|V_s|\times|V_x|}=\frac{ \sum_{i=1}^n v_{s,i}\times v_{x,i}}{\sqrt{\sum_{i=1}^n v_{s,i}^2}\times\sqrt{\sum_{i=1}^n v_{x,i}^2}}$$
4. 最後就可以算出平均餘弦相似係數。
#### 文章向量計算範例:

|文件內容|
|:----------------------------------------------------------------------------------------:|
|yuan ze university is a good university this university has many very good students|
|there are many students in yuan ze university many students are very good|
|there are good books in yuan ze university students love to read these books|
| there are many good teachers in yuan ze university these teachers concern these students|

計算出的詞頻（term frequency）
<img src="https://i.imgur.com/6yH53uE.png">

各別的文章向量

$$V_s=[1,1,3,1,1,2,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0]$$

$$V_1=[1,1,1,0,0,1,0,0,2,2,1,2,1,1,0,0,0,0,0,0,0]$$

$$V_2=[1,1,1,0,0,1,0,0,0,1,1,1,1,0,2,1,1,1,1,0,0]$$

$$V_3=[1,1,1,0,0,1,0,0,1,1,1,1,1,0,0,0,0,0,2,2,1]$$
### 輸入文件格式
```
文件ID
文件內容
```
#### 輸入文件範例
```
0001 
this is a book 
0002
this is a pen
0003
a good book is a book
0004
that book is good
```
### 執行範例
```
> g++ filename.cpp -o filename -lpthread
> ./filename.cpp input.txt
```
