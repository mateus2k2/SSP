from collections import Counter
import random
import loadData as ld

class ProcessingTimeGenerator:
    def __init__(self):
        numbers = self.getAllProcessTime(ld.loadJobs("250.csv")) + self.getAllProcessTime(ld.loadJobs("750.csv")) + self.getAllProcessTime(ld.loadJobs("1000.csv"))
        self.distribution = self.calculate_distribution(numbers)

    def getAllProcessTime(self, jobsDict):
        processTime = []
        for job in jobsDict:
            processTime.append(job['Processing Time'])
        return processTime

    def calculate_distribution(self, numbers):
        frequency = Counter(numbers)
        total_numbers = len(numbers)
        distribution = {number: count / total_numbers for number, count in frequency.items()}
        return distribution

    def generate_random_numbers(self, n):
        numbers = list(self.distribution.keys())
        probabilities = list(self.distribution.values())
        random_numbers = random.choices(numbers, weights=probabilities, k=n)
        return random_numbers
    