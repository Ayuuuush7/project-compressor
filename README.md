# 📦 Huffman Code File Compressor

A lossless data compression utility built in C++ based on the Huffman Coding algorithm. This project demonstrates core Data Structures and Algorithms (DSA) concepts to compress text files efficiently without memory leaks.

## 🚀 Features
* **Memory-Safe Compression:** Uses bit manipulation and a streaming buffer to prevent RAM overflow, even for large files.
* **Optimal Encoding:** Implements a Min-Heap (Priority Queue) to build a Huffman Tree, ensuring the most frequent characters get the shortest binary codes.
* **Full Decompression:** Reconstructs the exact original file from the compressed binary data using reverse mapping.

## 🛠️️ Tech Stack & DSA Used
* **Language:** C++
* **Data Structures:** Hash Maps (`std::unordered_map`), Priority Queue / Min-Heap, Binary Tree.
* **Core Concepts:** Greedy Algorithm, Bitwise Operations (Bit Packing), File I/O, Depth First Search (DFS).

## 💻 How to Run

1. Clone the repository:
   ```bash
   git clone [https://github.com/your-username/your-repo-name.git](https://github.com/your-username/your-repo-name.git)