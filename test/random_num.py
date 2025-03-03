import numpy as np

def generate_random_numbers(file_name, num_elements=10**6):
    if num_elements > 1e10:
        raise ValueError("Cannot generate more than 100,000 unique numbers in range [0, 100000]")

    numbers = np.random.permutation(int(1e6))[:num_elements]  # Generate unique numbers in range [0, 100000]
    
    with open(file_name, "w") as f:
        f.writelines(f"{num} " for num in numbers)  # Write numbers separated by spaces

generate_random_numbers("random_numbers.txt", num_elements=10**7)  # Generates 100,000 unique numbers

