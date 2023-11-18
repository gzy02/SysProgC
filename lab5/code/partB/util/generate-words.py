import os
import random
import string
import sys
from multiprocessing import Process

def generate_random_word():
    length = random.randint(3, 10)
    letters = random.choices(string.ascii_lowercase, k=length)
    return "".join(letters)

def generate_words(file_count, file_size):
    def worker(file_index):
        filename = "./../data/" + chr(ord('a') + file_index) + ".txt"
        with open(filename, "w") as f:
            current_size = 0
            while current_size < file_size:
                word = generate_random_word()
                f.write(word + " ")
                current_size += len(word) + 1

    data_dir = "./../data"
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)

    processes = []
    for i in range(file_count):
        p = Process(target=worker, args=(i,))
        processes.append(p)
        p.start()

    for p in processes:
        p.join()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python generate_words.py <file_count> <file_size>")
        sys.exit(1)

    file_count = int(sys.argv[1])
    file_size = int(sys.argv[2].upper().replace("M", "")) * 1024 * 1024

    generate_words(file_count, file_size)
