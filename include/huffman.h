#include "head.h"

class HuffmanEncode {
struct Node {
    int count;
    int left;
    int right;
    int removed;
};
public:
    HuffmanEncode();
    int encode(Mat A, string path);
    ~HuffmanEncode();
private:
    int findMinNode(Node* nodes, int length);
    int buildTree(Node* nodes, int* counts);
    void buildTable(Node* nodes, int pos, string bits, string * table);
};

class HuffmanDecode {
public:
    HuffmanDecode();
    Mat decode(string path, int height, int width);
    ~HuffmanDecode();
private:
    void decodeString(string & total, map<string, int> & table_map, int total_bit_length, Mat &res, int rows, int cols);
};