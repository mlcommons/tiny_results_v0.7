
# Imports
import glob
import torch
from brevitas_examples import bnn_pynq
import brevitas.onnx as bo
from finn.core.modelwrapper import ModelWrapper
import os
from finn.transformation.infer_shapes import InferShapes
from finn.transformation.insert_topk import InsertTopK
from finn.transformation.fold_constants import FoldConstants
from finn.transformation.general import (
    GiveReadableTensorNames,
    GiveUniqueNodeNames,
    RemoveStaticGraphInputs,
)
from finn.transformation.infer_datatypes import InferDataTypes
from finn.transformation.merge_onnx_models import MergeONNXModels
from finn.core.datatype import DataType

# Functions
from torch.nn import Module
class ToTensor(Module):
    def __init__(self):
        super(ToTensor, self).__init__()

    def forward(self, x):
        x = x / 255
        return x

# Find the model
checkpoint_list = glob.glob("experiments/*/checkpoints/best.tar")
checkpoint_list.sort()
latest_checkpoint = checkpoint_list[-1]
print(f"Exporting model from: {latest_checkpoint}")

# Load the model
trained_state_dict = torch.load(latest_checkpoint)["state_dict"]
brev_model = bnn_pynq.cnv_1w1a(pretrained=False)
brev_model.load_state_dict(trained_state_dict, strict=False)

# Export model to FINN-ONNX
ishape = (1, 3, 32, 32)
chkpt_name = "cnv-w1a1_retrained_no_pre-proc.onnx"
bo.export_finn_onnx(brev_model, ishape, chkpt_name)

# Add pre-processing
model = ModelWrapper(chkpt_name)
global_inp_name = model.graph.input[0].name
ishape = model.get_tensor_shape(global_inp_name)
# preprocessing: torchvision's ToTensor divides uint8 inputs by 255
totensor_pyt = ToTensor()
chkpt_preproc_name = "preproc.onnx"
bo.export_finn_onnx(totensor_pyt, ishape, chkpt_preproc_name)
assert os.path.isfile(chkpt_preproc_name)
# join preprocessing and core model
pre_model = ModelWrapper(chkpt_preproc_name)
pre_model = pre_model.transform(InferShapes())
pre_model = pre_model.transform(FoldConstants())
model = model.transform(MergeONNXModels(pre_model))
# add input quantization annotation: UINT8 for all BNN-PYNQ models
global_inp_name = model.graph.input[0].name
model.set_tensor_datatype(global_inp_name, DataType["UINT8"])
# postprocessing: insert Top-1 node at the end
model = model.transform(InsertTopK(k=1))
chkpt_name_final = "cnv-w1a1_retrained.onnx"
# tidy-up again
model = model.transform(InferShapes())
model = model.transform(FoldConstants())
model = model.transform(GiveUniqueNodeNames())
model = model.transform(GiveReadableTensorNames())
model = model.transform(InferDataTypes())
model = model.transform(RemoveStaticGraphInputs())
model.save(chkpt_name_final)

# Remove left over intermediate models
os.remove(chkpt_name)
os.remove(chkpt_preproc_name)

print(f"Exported retrained model to: {chkpt_name}")

