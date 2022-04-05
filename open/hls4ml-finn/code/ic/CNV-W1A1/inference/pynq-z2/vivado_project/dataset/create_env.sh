conda create --copy --name finn-env python=3.6

source activate finn-env

conda install -c anaconda scikit-learn h5py pyyaml keras
