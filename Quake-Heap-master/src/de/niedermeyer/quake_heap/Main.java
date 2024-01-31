// package de.niedermeyer.quake_heap;

import java.util.ArrayList;
import java.util.List;

public class Main {

    private static List<Leaf> entries = new ArrayList<>();

    public static void main(String[] args) throws Exception {
        QuakeHeap quakeHeap = new QuakeHeap();

        // System.out.println(quakeHeap);
        // entries.add(quakeHeap.insert(2, 2));
        // System.out.println(quakeHeap);
        // entries.add(quakeHeap.insert(3, 3));
        // entries.add(quakeHeap.insert(4, 4));
        // System.out.println(quakeHeap);
        // entries.remove(quakeHeap.extractMin());
        // System.out.println(quakeHeap);
        // quakeHeap.decreaseKey(entries.get(1), 2);
        // System.out.println(quakeHeap);
        // entries.remove(quakeHeap.extractMin());
        // System.out.println(quakeHeap);
        // insert 100 elements in deacrease order

        System.out.println("Initial Heap:");
        System.out.println(quakeHeap);

        for (int i = 300; i > 200; i--) {
            entries.add(quakeHeap.insert(i, i));
        }

        System.out.println("Heap after adding elements:");
        System.out.println(quakeHeap);

        // Decrease key operation from 200 to 150 by 100
        for (int i = 0; i < 50; i++) {
            quakeHeap.decreaseKey(entries.get(i), entries.get(i).getKey() - 100);
        }

        System.out.println("Heap after decrease key operations:");
        System.out.println(quakeHeap);

        System.out.println("Extracting elements in decreasing order:");

        for (int i = 100; i > 0; i--) {
            Leaf value = quakeHeap.extractMin();
            System.out.println(value.getKey());
            entries.remove(value);
        }
    }
}
