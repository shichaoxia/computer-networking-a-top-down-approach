import numpy as np
import math
import matplotlib.pyplot as plt


def vectorize(func):
    def wrapper(*args, **kw):
        return np.vectorize(func(*args, **kw))
    return wrapper


@vectorize
def slot_aloha_eff(n):
    return lambda p: n * p * math.pow(1 - p, n - 1)


@vectorize
def pure_aloha_eff(n):
    return lambda p: n * p * math.pow(1 - p, 2 * (n - 1))


def main():
    x = np.arange(0, 1, 0.01)
    plt.xlabel('probability')
    plt.ylabel('efficiency')
    plt.xlim(0, 0.4)
    plt.plot(x, slot_aloha_eff(15)(x), 'r', label='N=15 slotted')
    plt.plot(x, slot_aloha_eff(25)(x), 'g', label='N=25 slotted')
    plt.plot(x, slot_aloha_eff(35)(x), 'b', label='N=35 slotted')
    plt.plot(x, pure_aloha_eff(15)(x), 'r--', label='N=15 pure')
    plt.plot(x, pure_aloha_eff(25)(x), 'g--', label='N=25 pure')
    plt.plot(x, pure_aloha_eff(35)(x), 'b--', label='N=35 pure')
    plt.legend()
    plt.show()


if __name__ == '__main__':
    main()
