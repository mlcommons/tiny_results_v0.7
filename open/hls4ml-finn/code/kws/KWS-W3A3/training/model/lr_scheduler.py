# from torch.optim.lr_scheduler import _LRScheduler
# WARMUP_RATIO = 0.1
# MIN_LR = 1e-3
# HOLD_RATIO = 0.0
# POWER = 2.0
#
# def poly_decay(initial_lr, step, decay_steps, power, min_lr):
#     if step < 0:
#         warmup_lr = initial_lr * (step + 1) / (self.warmup_steps + 1)
#         return warmup_lr
#
#     # Hold phase
#     if (step >= self.warmup_steps) and (step < self.hold_steps):
#         return initial_lr
#
#     step = min(step, decay_steps)
#     p = step / decay_steps
#     lr = (initial_lr - min_lr) * math.pow(1.0 - p, power)
#     lr += min_lr
#     return lr
#
# class PolynomialHoldDecayAnneleaing(_LRScheduler):
#     def __init__(self, optimizer, warmup_steps=None, warmup_ratio=None, hold_steps=None, hold_ratio=None, total_steps=None, min_lr=0.0,
#     power=1.0, cycle=False):
#
#         self.total_steps = total_steps
#         if warmup_steps is not None:
#             self.warmup_steps = warmup_steps
#         elif warmup_ratio is not None:
#             self.warmup_steps = int(warmup_ratio * total_steps)
#         else:
#             self.warmup_steps = 0
#
#         # assert not (hold_steps is not None and hold_ratio is not None), "Either use particular number of step or ratio"
#         # assert hold_ratio is None or total_steps is not None, "If there is a ratio, there should be a total steps"
#
#         self._min_lr = min_lr
#         self._last_warmup_lr = 0.0
#
#         if hold_steps is not None:
#             self.hold_steps = hold_steps + self.warmup_steps
#         elif hold_ratio is not None:
#             self.hold_steps = int(hold_ratio * total_steps) + self.warmup_steps
#         else:
#             self.hold_steps = 0
#
#         self.min_lr = min_lr
#         self.power = power
#         self.cycle = cycle
#
#     def get_lr(self, initial_lr, step, epoch):
#         # Warmup phase
#         if step < self.warmup_steps:
#             warmup_lr = initial_lr * (step + 1) / (self.warmup_steps + 1)
#             return warmup_lr
#
#         # Hold phase
#         if (step >= self.warmup_steps) and (step < self.hold_steps):
#             return initial_lr
#
#         # if step > self.total_steps:
#         #     return self._min_lr
#         return self._get_lr(initial_lr, step, epoch)
#
#     def _get_lr(self, initial_lr, step, epoch):
#         return _poly_decay(
#             initial_lr,
#             step=step - self.hold_steps,
#             decay_steps=self.total_steps - max(self.warmup_steps, self.hold_steps),
#             power=self.power,
#             min_lr=self.min_lr,
#             cycle=self.cycle,
#         )
#
#
# def exp_lr_scheduler(optimizer, epoch, init_lr=BASE_LR, lr_decay_epoch=EPOCH_DECAY):
#     """Decay learning rate by a factor of DECAY_WEIGHT every lr_decay_epoch epochs."""
#     lr = init_lr * (DECAY_WEIGHT**(epoch // lr_decay_epoch))
#
#     if epoch % lr_decay_epoch == 0:
#         print('LR is set to {}'.format(lr))
#
#     for param_group in optimizer.param_groups:
#         param_group['lr'] = lr
#
#     return optimizer
import math

initial_lr = 0.05
min_lr = 0.001
warmup_steps = 0.05 * (200 * 445)
hold_steps = 0.45 * (200 * 445) + warmup_steps
decay_steps = (200*445) - max(warmup_steps, hold_steps)
def lr_scheduler(optimizer, step):
    if step < warmup_steps:
        warmup_lr = initial_lr * (step + 1) / (warmup_steps + 1)
        final_lr = warmup_lr
    # Hold phase
    elif (step >= warmup_steps) and (step < hold_steps):
        final_lr = initial_lr
    else:
        step = step-hold_steps
        step = min(step, decay_steps)
        p = step / decay_steps
        lr = (initial_lr - min_lr) * math.pow(1.0 - p, 2.0)
        lr += min_lr
        final_lr = lr
    for param_group in optimizer.param_groups:
        param_group['lr'] = final_lr
    return optimizer
