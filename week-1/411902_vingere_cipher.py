from email import message


mat = [[j for j in range(26)] for i in range(26)]


def changer(s, key):
    k = len(s)/len(key)
    while(k > 1):
        key += key
        k = len(s)/len(key)
        while(len(key) != len(s)):
            key = key[:-1]
    return key


def encrypt(s, key):
    cipher = ""
    s = s.lower()
    key = key.lower()
    newkey = changer(s, key)
    for i in range(len(s)):
        l = ord(s[i])-97
        k = ord(newkey[i])-97
        cipher += chr((mat[l][k]+ord(s[i])-97) % 26+97)
    return cipher.upper()


def decrypt(s, key):
    plain = ""
    s = s.lower()
    key.lower()
    newkey = changer(s, key)
    for i in range(len(s)):
        c = (ord(s[i])-ord(newkey[i])+26) % 26
        plain += chr(c+97)
    return plain


key = input("Enter key: ")
message = input("Enter message: ")
eccryptedMessage = encrypt(message, key)
decryptedMessage = decrypt(eccryptedMessage, key)

print(f'message: {message}, key: {key}, encrypted message: {eccryptedMessage}, decrypted message: {decryptedMessage}')
