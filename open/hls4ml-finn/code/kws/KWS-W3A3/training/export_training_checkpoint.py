import os
import shutil
from determined.experimental import client

# Log into the determined.ai instance
client.login(master="localhost", user="determined", password="")

# Get the experiment
with open("experiment_number.txt", "r") as f:
    exp_number = int(f.readline())
print(f"Getting expriment with ID {exp_number}")
exp = client.get_experiment(exp_number)
print("Waiting for experiment to complete")
completion_reason = exp.wait()
if completion_reason.name == "COMPLETED":
    print("Training completed succesfully")
else:
    print(f"The experiment exited with status: {completion_reason.name}")
    print("The training thus didn't completed succesfully. Exiting here.")
    exit(2)

# Get the last checkpoint
export_path = "training_checkpoint"
print(f"Exporting checkpoint to folder: '{export_path}'")
if os.path.exists(export_path) and os.path.isdir(export_path):
    print(f"Checkpoint folder '{export_path}' already exists, removing.")
    shutil.rmtree(export_path)
# Due to the incompleteness of the API we will need to go through the "top" checkpoint to get the trial id,
# to then search for the latest recent checkpoint.
chk_top = exp.top_checkpoint()
trail_id = chk_top.trial_id
trial = client.get_trial(trail_id)
chk = trial.select_checkpoint(latest=True)

print(f"Downloading checkpoint with uuid: {chk.uuid}")
chk.download(export_path)
