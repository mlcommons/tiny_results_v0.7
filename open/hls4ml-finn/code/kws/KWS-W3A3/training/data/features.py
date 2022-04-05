import torch
import torch.nn as nn

class CropOrPadSpectrogramAugmentation(nn.Module):
    """
    Pad or Crop the incoming Spectrogram to a certain shape.

    Args:
        audio_length (int): the final number of timesteps that is required.
            The signal will be either padded or cropped temporally to this
            size.
    """

    def __init__(self, audio_length):
        super(CropOrPadSpectrogramAugmentation, self).__init__()
        self.audio_length = audio_length

    @torch.no_grad()
    def forward(self, input_signal):
        image = input_signal
        num_images = image.shape[0]

        audio_length = self.audio_length
        image_len = image.shape[-1]

        # Crop long signal
        if image_len > audio_length:  # randomly slice
            cutout_images = []
            offset = torch.randint(low=0, high=image_len - audio_length + 1, size=[num_images])

            # TODO: Look into advanced broadcasting to speed up section
            for idx, offset in enumerate(offset):
                cutout_images.append(image[idx : idx + 1, :, offset : offset + audio_length])

            image = torch.cat(cutout_images, dim=0)
            del cutout_images

        else:  # symmetrically pad short signal with zeros
            pad_left = (audio_length - image_len) // 2
            pad_right = (audio_length - image_len) // 2

            if (audio_length - image_len) % 2 == 1:
                pad_right += 1

            image = torch.nn.functional.pad(image, [pad_left, pad_right], mode="constant", value=0)

        # Replace dynamic length sequences with static number of timesteps

        return image
