
class CeaserCipher:
    def encrypt(self, message, key):
        res = ''
        for char in message:
            num = ord(char) - 97
            num += key
            num = num % 26
            newKey = 97 + num
            res += chr(newKey)
        return res.upper()

    def decrypt(self, message, key):
        message = message.lower()
        res = ''
        for char in message:
            num = ord(char) - 97
            num -= key
            num = ((26 + num) % 26) + 97
            res += chr(num)
        return res


if __name__ == "__main__":
    message = 'lalithadityaanchula'
    key = 2
    encryptedMsg = CeaserCipher().encrypt(message, key)
    decryptedMsg = CeaserCipher().decrypt(encryptedMsg, key)
    print(f'key :{key} ,message : {message}')
    print(
        f'encryptedMessage : {encryptedMsg}, decryptedMessage : {decryptedMsg}')
