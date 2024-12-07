
#include "Huffman.h"
#include "cmath"

Huffman::Huffman(string file): fileName(file){}

Huffman::~Huffman()
{

}

void Huffman::Frequencies(string file)
{
    /*
     *This function takes in the file to be compressed and records the frequencies of each character
     by adding it to a map when encountering a char for the first time then incrementing the count every
     other time encountered
     */
    ifstream infile(file);
    if (!infile.is_open()) {
        cerr << "Error: Unable to open file."<< endl;
    }

    unordered_map<char, int> freqmap;
    int total = 0;
    char ch;
    while (infile.get(ch))
    {

            if (freqmap.find(ch) == freqmap.end())
            {
                freqmap.insert(make_pair(ch, 1));
            }
            else
            {
                freqmap[ch]++;
            }
            total++;

    }
    infile.close();

/*
    for (auto& it : freqmap) {
        if (std::isprint(it.first)) {
            std::cout << "'" << it.first << "' : " << it.second << '\n';
        } else {
            std::cout << "Character: " << ch << " ASCII: " << static_cast<int>(it.first) << '\n'
                      << " : " << it.second << '\n';
        }
    }
    cout<<"total: "<<total<<endl;
    */

    //this was used to test frequencies


    for (auto& it : freqmap)
    {
        pq.insert(it.first, it.second);
    }
    //adding to priority queue


}

void Huffman::buildHuffmanTree()
{
/*
 Here we follow the principle rules of huffman trees, we dequeue the two elements with the least frequencies
 then join them to a parent intenral node, then insert the new parent node back into the priority qurur
 it loops this until there only remains the parent node of the tree, which is assigned to "root"
 */
    while(pq.size>1)
    {
        Node<char> *FirstNode = pq.dequeue();
        Node<char> *SecondNode = pq.dequeue();


        Node<char> *ParentNode = new Node<char>( -999, FirstNode->priori + SecondNode->priori);
        ParentNode->right = SecondNode;
        ParentNode->left = FirstNode;
        pq.insert(ParentNode);
    }
    root = pq.dequeue();

}

void Huffman::CreateEncodings(Node<char>* temproot, string enc)
{
    /*
     This is a recursive function that loops through all the leaf nodes and assigns them the
     correct binary code by adding "1" whenever we go right and a "0" whenever we go left
     */
    if((temproot->left == nullptr)&&(temproot->right == nullptr))
    {
        vector<bool> newVector;

        for(int i=0; i<enc.size(); i++)
        {
            if(enc[i] == '1')
            {
                newVector.push_back(1);
            }
            else
            {
                newVector.push_back(0);
            }
        }
        Encodings.insert(make_pair(temproot->data, newVector));
        return;
    }
    else
    {
        CreateEncodings(temproot->left, enc + '0');
        CreateEncodings(temproot->right, enc + '1');
    }
}

void Huffman::EncodeTreeNormal(string file)
{
    /*
     This function simply writes down the frequencies in order of the priority queue used during
     creating the tree to create the same exact tree when decompressing
     */
    ofstream outfile(file);

    Node<char>* temproot = pq.root;
    int size = pq.size;
    string sizeString = to_string(size);
    outfile<<sizeString;
    int charnum = 0;

    while(temproot!=NULL)
    {
        char ch = temproot->data;
        int freq = temproot->priori;
        outfile<<ch;
        outfile<<to_string(freq);
        charnum++;
        temproot = temproot->next;

    }
    outfile<<'\n';
    outfile.close();

}
void Huffman::EncodeTreeCaeser(string file)
{

    /*
     This function simply writes down the frequencies in order of the priority queue used during
     creating the tree to create the same exact tree when decompressing but uses a caeser cypher method
     of encryption by shifting the characters by a fixed amount encoded at the beginning of the file
     */
    ofstream outfile(file);
    int randcypher = rand() % 4 + 1;

    outfile<<to_string(randcypher);

    Node<char>* temproot = pq.root;

    while(temproot!=NULL)
    {
        char ch = temproot->data;
        int freq = temproot->priori;
        char newch = (ch - randcypher+256)%256;
        outfile<<newch;
        outfile<<to_string(freq);
        temproot = temproot->next;

    }
    outfile<<'\n';
    outfile.close();

}

void Huffman::ReadTreeNormal(string file)
{
    /*
     This function reads the frequencies from the compressed file and also gets the value of the padding
     which is important when reading the last character
     */
    chars = 0;
    int pqsize;
    string pqsizestring = "";
    ifstream infile1(file);
    int numchar = 0;
    char pad;

    while(isdigit(infile1.peek()))
    {
        pqsizestring += infile1.get();
    }
    pqsize = stoi(pqsizestring);

    while(infile1.peek()!= EOF)
    {
        pad = infile1.get();
    }
    padding = pad - '0';
    infile1.close();

    ifstream infile(file);

    for(int i=0; i<pqsizestring.size(); i++)
    {
        infile.get();
        chars++;
    }


   while(pq.size<pqsize)
   {
       char ch = infile.get();
       chars++;
       string num = "";
       while(isdigit(infile.peek()))
       {
           num+=infile.get();
           chars++;
       }
       if(num!="")
       {
           long intnum = stol(num);

           pq.insert(ch, intnum);
           numchar++;
       }
       else
       {
           break;
       }

   }



}

void Huffman::ReadTreeCaeser(string file)
{
    /*
     This function reads the frequencies from the compressed file and also gets the value of the padding
     which is important when reading the last character, it also shifts back the characters to their correct
     true value before caeser encryption
     */
    chars = 0;

    // First, read the entire file to extract padding
    ifstream infile1(file);

    char pad;
    while (infile1.peek() != EOF)
    {
        pad = infile1.get();
    }
    padding = pad - '0';

    infile1.close();

    ifstream infile(file);

    // Read the cipher key
    char cypher = infile.get();
    chars++;

    int cypherkey = cypher - '0';

    while (infile.peek() != '\n')
    {
        chars++;
        char wrongch = infile.get();

        char ch = (wrongch + cypherkey) % 256; //this was the only way the true char is back after debugging
        if (ch < 0)
        {ch += 256;}

        string num = "";
        while (isdigit(infile.peek()))
        {
            chars++;
            num += infile.get();
        }

        if (num != "")
        {
            int intnum = stoi(num);
            pq.insert(ch, intnum);
        }
        else
        {
            break;
        }
    }
}

void Huffman::Compress(string type) {

    Frequencies(fileName); //constructs priority queue of frequencies
    if(type == "Caeser")
    {
        EncodeTreeCaeser("Compressed.txt"); //writes down the frequencies at top of file using caeser
    }
    else if(type == "Normal")
    {
        EncodeTreeNormal("Compressed.txt"); //writes down the frequencies at top of file
    }
    else
    {
        cerr<<"You need to choose either caeser or normal encryption"<<endl;
    }

    buildHuffmanTree();// builds tree

    CreateEncodings(root, ""); //creating encodings map


    ifstream infile(fileName, ios::binary);
    ofstream outfile("Compressed.txt", ios::app);

    if (!infile.is_open()) {
        cerr << "Error: Unable to open input file '" << fileName << "'." << endl;
        return;
    }

    unsigned char buffer = 0;
    int bitCount = 0;

    char ch;
    while (infile.get(ch)) {
        /*
         tHis part was particularly difficult and needed google and ai assistance
         it is the only way to output individual bits not bytes, also used in decompression
         */
        vector<bool> encoding = Encodings[ch];

        for (bool bit : encoding) {
            buffer = buffer * 2 + bit;
            bitCount++;

            if (bitCount == 8) {
                outfile.put(buffer);
                buffer = 0;
                bitCount = 0;
            }
        }
    }
//if there remains couple bits to be added, we pad them and store that padding bc its important
    if (bitCount > 0) {
        padding = 8-bitCount;
        buffer = buffer << (8 - bitCount);
        outfile.put(buffer);
    }
    outfile<<to_string(padding); //store padding


    infile.close();
    outfile.close();
}
void Huffman::Decompress(string type) {
    if(type == "Caeser")
    {
        ReadTreeCaeser(fileName); //read frequencies into priority queue after cyphering
    }
    else if(type == "Normal")
    {
        ReadTreeNormal(fileName); //read frequencies into priority queue
    }
    else
    {
        cerr<<"You need to choose either caeser or normal encryption"<<endl;
    }
    buildHuffmanTree(); //builds tree similar to compression

    ifstream infile(fileName, ios::binary);
    ofstream outfile("Decompressednewfile.txt");

    if (!infile.is_open()) {
        cerr << "Error: Unable to open compressed file." << endl;
        return;
    }

    for(int i=0; i<chars+1; i++)
    {
        infile.get();
    }


    char ch;
    unsigned char buffer;

    Node<char>* currentNode = root;

    while (infile.get(reinterpret_cast<char&>(buffer))) {
        for (int i = 7; i >= 0; i--) {
            //same methodology of bits

            bool bit = (buffer >> i) & 1;  //getting the ith bit
            if(bit)
            {
                currentNode = currentNode->right;
            }
            else
            {
                currentNode = currentNode->left;
            }
            if ((currentNode->left == NULL)&& (currentNode->right == NULL)) {

                bool lastchar = false;
                char ch = infile.get();
                if(infile.peek()==EOF)
                {
                    lastchar = true;
                }
                streampos currentPosition = infile.tellg();
                infile.seekg(currentPosition -  static_cast<streamoff>(1));
                if (lastchar && i < padding)
                {
                    return;
                }
                else
                {
                    outfile.put(currentNode->data);
                    currentNode = root;
                }
            }
        }
    }

    infile.close();
    outfile.close();
}