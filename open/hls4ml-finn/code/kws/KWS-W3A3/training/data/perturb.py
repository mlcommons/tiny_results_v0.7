import numpy as np
import torch

class WhiteNoisePerturbation:
    def __init__(self, min_level=-90, max_level=-46, rng=None):
        self.min_level = int(min_level)
        self.max_level = int(max_level)
        self._rng = np.random.RandomState() if rng is None else rng

    def process(self, data):
        with torch.no_grad():
            noise_level_db = self._rng.randint(self.min_level, self.max_level, dtype='int32')
            noise_signal = self._rng.randn(data.shape[0]) * (10.0 ** (noise_level_db / 20.0))
            output = data + noise_signal
            return output
