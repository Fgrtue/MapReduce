import numpy as np

def generate_random_numbers(file_name, num_elements=10**7, chunk_size=10**5):
    with open(file_name, "w") as f:
        for _ in range(num_elements // chunk_size):  # Loop through chunks
            numbers = np.random.randint(0, int(1e10), size=chunk_size, dtype=np.int64)  # Generate chunk of random numbers
            f.writelines(f"{num} " for num in numbers)  # Write the chunk to file

generate_random_numbers("random_numbers.txt", num_elements=10**6)
