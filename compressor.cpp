#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue> // Min-Heap ke liye

using namespace std;

// 1. Node Structure (Tree banane ke liye)
struct Node {
    char data;
    int freq;
    Node* left;
    Node* right;

    // Constructor
    Node(char d, int f) {
        data = d;
        freq = f;
        left = right = nullptr;
    }
};

// 2. Custom Comparator Min-Heap ke liye
struct Compare {
    bool operator()(Node* l, Node* r) {
        // C++ priority_queue default Max-Heap hoti hai. 
        // '>' sign lagane se ye Min-Heap ban jayegi (Choti frequency upar aayegi)
        return l->freq > r->freq;
    }
};

class HuffmanCompressor {
private:
    // Private properties: Bahaar se koi isko direct change nahi kar sakta (Encapsulation)
    unordered_map<char, int> freqMap;
    Node* root; // Tree ka root pointer save karne ke liye

    // NAYA: Har character ka final binary string store karne ke liye
    unordered_map<char, string> huffmanCodes;

    // NAYA: Private recursive helper function DFS ke liye
    void generateCodesRec(Node* node, string code) {
        // Base Case: Agar node null hai, toh wapas jao
        if (node == nullptr) return;

        // Agar ye Leaf Node hai (left aur right null hain)
        if (node->left == nullptr && node->right == nullptr) {
            huffmanCodes[node->data] = code;
            return;
        }

        // Left jate waqt "0" append karo
        generateCodesRec(node->left, code + "0");
        
        // Right jate waqt "1" append karo
        generateCodesRec(node->right, code + "1");
    }

public:

    // Constructor (root ko null initialize karne ke liye)
    HuffmanCompressor() {
        root = nullptr;
    }
    // Function to read file and count character frequencies
    void buildFrequencyMap(const string& inputFileName) {
        // ifstream (Input File Stream) file read karne ke liye hota hai
        ifstream inputFile(inputFileName);

        // Edge Case Handling: Agar file exist nahi karti
        if (!inputFile.is_open()) {
            cerr << "Error: Could not open the file " << inputFileName << endl;
            return;
        }

        char ch;
        // inputFile.get(ch) ek-ek character read karta hai, including spaces (\s) aur newlines (\n)
        while (inputFile.get(ch)) {
            freqMap[ch]++; // Agar ch map me nahi hai, toh add karke 1 set karega, warna increment karega
        }

        inputFile.close();
        
        // Debugging ke liye (Baad mein isko hata denge)
        cout << "Frequencies calculated successfully. Unique characters: " << freqMap.size() << endl;
    }


    // --- PHASE 2: NEW FUNCTION ---
    void buildHuffmanTree() {
        // Step 1: Ek Min-Heap banao jo Node pointers store kare
        priority_queue<Node*, vector<Node*>, Compare> minHeap;

        // Step 2: Map ke saare characters ko Heap me daal do (as leaf nodes)
        for (auto pair : freqMap) {
            minHeap.push(new Node(pair.first, pair.second));
        }

        // Step 3: Tree build karo
        while (minHeap.size() > 1) {
            // Do sabse choti frequency wale nodes nikalo
            Node* leftNode = minHeap.top();
            minHeap.pop();

            Node* rightNode = minHeap.top();
            minHeap.pop();

            // Ek naya parent node banao. Isme character '$' (dummy) rakhte hain 
            // aur frequency dono bacchon ka sum hoti hai.
            int sum = leftNode->freq + rightNode->freq;
            Node* parentNode = new Node('$', sum);
            
            parentNode->left = leftNode;
            parentNode->right = rightNode;

            // Naye node ko wapas heap me daal do
            minHeap.push(parentNode);
        }

        // Step 4: Aakhiri bacha hua node hamara Root hai
        if (!minHeap.empty()) {
            root = minHeap.top();
        }
        
        cout << "Huffman Tree built successfully!" << endl;
    }

    // --- PHASE 3: NEW FUNCTIONS ---
    void generateCodes() {
        // Root se recursion start karo, initial code empty string "" hoga
        generateCodesRec(root, "");
        cout << "Huffman Codes generated successfully!" << endl;
    }
    // Debugging ke liye codes print karna
    void printCodes() {
        cout << "\n--- Generated Huffman Codes ---" << endl;
        for (auto pair : huffmanCodes) {
            if (pair.first == '\n') 
                cout << "\\n : " << pair.second << endl;
            else 
                cout << pair.first << " : " << pair.second << endl;
        }
        cout << "-------------------------------" << endl;
    }

    
    // Ek helper function taaki hum frequencies dekh sakein
    void printFrequencies() {
        for (auto const& pair : freqMap) {
            // \n ko directly print karne pe formatting kharab hoti hai, isliye condition lagayi
            if (pair.first == '\n') 
                cout << "\\n : " << pair.second << endl;
            else 
                cout << pair.first << " : " << pair.second << endl;
        }
    }

    // --- PHASE 4: NEW FUNCTION ---
    void compressToFile(const string& inputFileName, const string& outputFileName) {
        ifstream inputFile(inputFileName);
        // ios::binary lagana zaroori hai kyunki hum text nahi, raw bytes likh rahe hain
        ofstream outputFile(outputFileName, ios::binary); 
        
        if (!inputFile.is_open() || !outputFile.is_open()) {
            cerr << "Error opening files!" << endl;
            return;
        }

        // STEP 1: Metadata/Header likhna
        // Decompress karte waqt humein ye tree wapas chahiye hoga, isliye hum is map ko file ke shuru me likh dete hain.
        // Note: For simplicity in this project, we assume the user knows how to read this back.
        outputFile << huffmanCodes.size() << '\n';
        for (auto pair : huffmanCodes) {
            outputFile << pair.first << pair.second << '\n';
        }

        // STEP 2: Bit Packing aur Encoding (FIXED MEMORY BUG)
        string bitBuffer = ""; // Ye hamara temporary buffer hai
        char ch;
        
        while (inputFile.get(ch)) {
            // Character ka code buffer mein daalo
            bitBuffer += huffmanCodes[ch]; 
            
            // Jaise hi buffer mein 8 ya 8 se zyada bits ho jayein, unhe 1 Byte me pack karke file me likh do
            while (bitBuffer.size() >= 8) {
                unsigned char byte = 0; // 8 bits ka empty dabba (00000000)
                
                for (int i = 0; i < 8; i++) {
                    if (bitBuffer[i] == '1') {
                        // Bit manipulation: 1 ko shift karke OR (|) operation karo
                        byte = byte | (1 << (7 - i)); 
                    }
                }
                
                outputFile.put(byte); // Actual byte file me write ho gaya
                
                // Jo 8 bits likh diye, unhe buffer se hata do
                bitBuffer = bitBuffer.substr(8); 
            }
        }
        
        // STEP 3: Padding (Aakhiri bache hue bits)
        // Agar file end ho gayi par buffer me 3-4 bits bach gaye, toh unhe 8 ka multiple banane ke liye end me '0' lagane padenge
        int padding = 0;
        if (bitBuffer.size() > 0) {
            padding = 8 - bitBuffer.size();
            while (bitBuffer.size() < 8) {
                bitBuffer += "0";
            }
            
            unsigned char byte = 0;
            for (int i = 0; i < 8; i++) {
                if (bitBuffer[i] == '1') {
                    byte = byte | (1 << (7 - i));
                }
            }
            outputFile.put(byte);
        }

        // Interviewer ko impress karne ke liye metadata print karo
        inputFile.clear();
        inputFile.seekg(0, ios::end);
        int originalSize = inputFile.tellg();
        
        int compressedSize = outputFile.tellp();
        
        cout << "\n--- Compression Complete ---" << endl;
        cout << "Original Size: " << originalSize << " bytes" << endl;
        cout << "Compressed Size: " << compressedSize << " bytes" << endl;
        cout << "Space Saved: " << 100.0 - ((float)compressedSize / originalSize * 100) << "%" << endl;

        inputFile.close();
        outputFile.close();
    }

    // --- PHASE 5: NEW FUNCTION ---
    void decompressFromFile(const string& inputFileName, const string& outputFileName) {
        // Input binary mode mein, output normal text mode mein
        ifstream inputFile(inputFileName, ios::binary);
        ofstream outputFile(outputFileName);

        if (!inputFile.is_open() || !outputFile.is_open()) {
            cerr << "Error opening files for decompression!" << endl;
            return;
        }

        // STEP 1: Read the Header (Metadata)
        int mapSize;
        inputFile >> mapSize;
        inputFile.get(); // Size ke baad wale 'newline' character ko consume karne ke liye

        unordered_map<string, char> reverseCodes; // String code se Character nikalne ke liye
        
        for (int i = 0; i < mapSize; i++) {
            char ch;
            inputFile.get(ch); // Ek character padho (ye space ya newline bhi ho sakta hai)
            
            string code;
            getline(inputFile, code); // Uska binary code padho (newline aane tak)
            
            reverseCodes[code] = ch; // Reverse dictionary me save kar lo
        }

        // STEP 2: Read Binary Data and Decode (Bit Extraction)
        char byte;
        string currentCode = "";
        
        // File se ek-ek byte (8-bits) read karte raho jab tak file end na ho jaye
        while (inputFile.get(byte)) {
            
            // Ek Byte ke andar 8 bits hote hain, MSB (Most Significant Bit) se LSB tak check karo
            for (int i = 7; i >= 0; i--) {
                // Bitwise Right Shift aur AND (&) operator se actual bit nikalo
                int bit = (byte >> i) & 1; 
                
                // Bit ko string me convert karke currentCode me jodo
                currentCode += to_string(bit); 

                // Check karo ki kya ye code hamari dictionary me exist karta hai?
                if (reverseCodes.find(currentCode) != reverseCodes.end()) {
                    // Agar match mil gaya, toh character output file me likh do
                    outputFile.put(reverseCodes[currentCode]);
                    
                    // Naye character ke liye code string ko wapas khali kar do
                    currentCode = ""; 
                }
            }
        }

        cout << "\n--- Decompression Complete ---" << endl;
        cout << "Decoded file saved as: " << outputFileName << endl;

        inputFile.close();
        outputFile.close();
    }
};

int main() {
    HuffmanCompressor compressor;
    
    // Test karne ke liye ek dummy "test.txt" file bana lena jisme kuch text ho
    compressor.buildFrequencyMap("test.txt");
    compressor.printFrequencies();
    
    compressor.buildHuffmanTree(); // Tree build function call kiya

    // Naye functions call kiye
    compressor.generateCodes();
    compressor.printCodes();

    compressor.compressToFile("test.txt", "compressed.bin");


    // Decompress karo
    compressor.decompressFromFile("compressed.bin", "restored.txt");

    return 0;
}