import random
try:
    input = raw_input
except NameError:
    pass
try:
    chr = unichr
except NameError:
    pass


def random_prime_number_producer():
    num = []
    i = 2
    for i in range(2, 100):
        j = 2
        for j in range(2, i):
            if(i % j == 0):
                break
        else:
            num.append(i)
    return random.sample(num, 2)


def gcd(a, b):
    while b != 0:
        c = a % b
        a = b
        b = c
    return a


def modinv(a, m):
    for x in range(1, m):
        if (a * x) % m == 1:
            return x
    return None


def coprimes(a):
    l = []
    for x in range(2, a):
        if gcd(a, x) == 1 and modinv(x, phi) != None:
            l.append(x)
    for x in l:
        if x == modinv(x, phi):
            l.remove(x)
    return l


def generate_PK():
    global n, phi, e, d
    (p, q) = random_prime_number_producer()

    print("Choosen primes:\np=" + str(p) + ", q=" + str(q) + "\n")
    n = p * q
    print("n = p * q = " + str(n) + "\n")
    phi = (p - 1) * (q - 1)
    coprime_number = coprimes(phi)
    e = random.choice(coprime_number)
    d = modinv(e, phi)
    print("\nYour public key is a pair of numbers (e=" +
          str(e) + ", n=" + str(n) + ").\n")
    print("Your private key is a pair of numbers (d=" +
          str(d) + ", n=" + str(n) + ").\n")
    return (n, e), (n, d)


public_key, private_key = generate_PK()


def encrypt_block(m, key):
    (n, e) = key
    c = (m**e) % n
    if c == None:
        print('No modular multiplicative inverse for block ' + str(m) + '.')
    return c


def encrypt_string(s, key):
    return ''.join([chr(encrypt_block(ord(x), key)) for x in list(s)])


s = '42'
print("\nPlain message: " + s + "\n")
enc = encrypt_string(s, private_key)
print("Encrypted message: " + enc + "\n")
dec = encrypt_string(enc, public_key)
print("Decrypted message: " + dec + "\n")


Number = n
for i in range(2, Number + 1):
    if (Number % i == 0):
        isprime = 1
        for j in range(2, (i // 2 + 1)):
            if (i % j == 0):
                isprime = 0
                break

        if (isprime == 1):
            print(" %d is a Prime Factor of a Given Number %d" % (i, Number))



print('since n, e and the prime factors that construct n are all known, it is easy to get the private key  (n,d)')
print('Therefore, two bigger p and q shoule be used to get n '
      'so that it is difficult to factorize n')
