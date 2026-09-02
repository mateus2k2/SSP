from collections import Counter
from pathlib import Path
import random

from . import loadData as ld

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
PROCESSED_DIR = REPO_ROOT / "input" / "Processed"


class ProcessingTimeGenerator:
    """Samples processing times from the empirical distribution of real job
    data (input/Processed/{250,750,1000}.csv), so generated instances have
    processing-time distributions consistent with the real dataset instead
    of e.g. a uniform draw."""

    def __init__(self):
        numbers = []
        for fname in ("250.csv", "750.csv", "1000.csv"):
            numbers += self._processing_times(ld.loadJobs(str(PROCESSED_DIR / fname)))
        self.distribution = self._calculate_distribution(numbers)

    def _processing_times(self, jobs):
        return [job['Processing Time'] for job in jobs]

    def _calculate_distribution(self, numbers):
        frequency = Counter(numbers)
        total = len(numbers)
        return {number: count / total for number, count in frequency.items()}

    def generate_random_numbers(self, n):
        numbers = list(self.distribution.keys())
        probabilities = list(self.distribution.values())
        return random.choices(numbers, weights=probabilities, k=n)
