#include "quake_heap.hpp"
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <queue>
using namespace std;

int main()
{
	const unsigned int N = 1000000;
	using QH = QuakeHeap<int, double>;
	QH pq;
	using EntryIterator = std::unordered_map<int, QH::Entry>::iterator;
	std::unordered_map<int, QH::Entry> entries;
	entries.reserve(N);

	priority_queue<int> stdpq;
	auto startInsertionstdpq = chrono::high_resolution_clock::now();
	for (unsigned long i = 2000; i < N; i++)
	{
		stdpq.push(i);
	}
	auto endInsertionstdpq = chrono::high_resolution_clock::now();
	chrono::duration<double> insertionTimestdpq = endInsertionstdpq - startInsertionstdpq;
	cout << "Insertion time stdpq: " << insertionTimestdpq.count() << " seconds" << endl;

	// extract all the entries from the heap
	auto startExtractionstdpq = chrono::high_resolution_clock::now();
	while (!stdpq.empty())
	{
		stdpq.pop();
	}
	auto endExtractionstdpq = chrono::high_resolution_clock::now();
	chrono::duration<double> extractionTimestdpq = endExtractionstdpq - startExtractionstdpq;
	cout << "Extraction time stdpq: " << extractionTimestdpq.count() << " seconds" << endl;

	auto startInsertion = chrono::high_resolution_clock::now();
	// insert N entries from 2000 to N
	for (unsigned long i = 2000; i < N; i++)
	{
		entries[i] = QH::makeEntry(i, i);
		pq.insert(entries[i]);
	}
	auto endInsertion = chrono::high_resolution_clock::now();
	chrono::duration<double> insertionTime = endInsertion - startInsertion;
	cout << "Insertion time: " << insertionTime.count() << " seconds" << endl;

	// decrease key of every by 2000 and erase from entries and update in pq
	// count time
	auto startDecreaseKey = chrono::high_resolution_clock::now();
	for (int i = 2000; i < N; i++)
	{
		pq.decreaseKey(entries[i], i - 2000);
		entries[i - 2000] = entries[i];
	}
	auto endDecreaseKey = chrono::high_resolution_clock::now();
	chrono::duration<double> decreaseKeyTime = endDecreaseKey - startDecreaseKey;
	cout << "Decrease key time: " << decreaseKeyTime.count() << " seconds" << endl;

	// extract all the entries from the heap
	auto startExtraction = chrono::high_resolution_clock::now();
	while (!pq.empty())
	{
		pq.deleteMin();
	}
	auto endExtraction = chrono::high_resolution_clock::now();
	chrono::duration<double> extractionTime = endExtraction - startExtraction;
	cout << "Extraction time: " << extractionTime.count() << " seconds" << endl;
	return 0;
}