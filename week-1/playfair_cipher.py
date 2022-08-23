from email import message


def getMatrix(key):
    charToPos = dict()
    posToChar = dict()
    seen = set()
    if 'i' in key:
        rejI = False
    else:
        rejI = True
    row, col = 0, 0
    for char in key:
        if char in seen:
            continue
        charToPos[char] = (row, col)
        posToChar[(row, col)] = char
        col += 1
        if col == 5:
            row += 1
            col = 0
        seen.add(char)

    for i in range(97, 97 + 26):
        char = chr(i)
        if char in seen:
            continue
        if rejI and char == 'i':
            continue
        charToPos[char] = (row, col)
        posToChar[(row, col)] = char
        col += 1
        if col == 5:
            row += 1
            col = 0
        seen.add(char)
    return charToPos, posToChar


def getDigraphs(message):
    pointer = 0
    pairs = []
    while pointer < len(message):
        one, two = pointer, pointer + 1
        if two == len(message):
            pairs.append(message[one] + 'z')
            pointer += 1
        else:
            if message[one] == message[two]:
                pairs.append(message[one] + 'x')
                pointer += 1
            else:
                pairs.append(message[one] + message[two])
                pointer += 2
    return pairs


class PlayFair:
    def encrypt(self, message, key):
        charToPos, posToChar = getMatrix(key)
        pairs = getDigraphs(message)
        res = ''
        for one, two in pairs:
            rowOne, colOne = charToPos[one]
            rowTwo, colTwo = charToPos[two]
            if colOne == colTwo:
                resOne, resTwo = (rowOne + 1) % 5, (rowTwo + 1) % 5
                res = res + posToChar[(resOne, colOne)] + \
                    posToChar[(resTwo, colOne)]
            elif rowOne == rowTwo:
                resOne, resTwo = (colOne + 1) % 5, (colTwo + 1) % 5
                res = res + posToChar[(rowOne, resOne)] + \
                    posToChar[(rowOne, resTwo)]
            else:
                res += posToChar[(rowOne, colTwo)]
                res += posToChar[(rowTwo, colOne)]
        return res.upper()

    def decrypt(self, message, key):
        message = message.lower()
        pairs = getDigraphs(message)
        charToPos, posToChar = getMatrix(key)
        res = ''
        for one, two in pairs:
            rowOne, colOne = charToPos[one]
            rowTwo, colTwo = charToPos[two]
            if colOne == colTwo:
                resOne, resTwo = (rowOne - 1 + 5) % 5, (rowTwo - 1 + 5) % 5
                res = res + posToChar[(resOne, colOne)] + \
                    posToChar[(resTwo, colOne)]

            elif rowOne == rowTwo:
                resOne, resTwo = (colOne - 1 + 5) % 5, (colTwo - 1 + 5) % 5
                res = res + posToChar[(rowOne, resOne)] + \
                    posToChar[(rowOne, resTwo)]
            else:
                res += posToChar[(rowOne, colTwo)]
                res += posToChar[(rowTwo, colOne)]
        return res


if __name__ == "__main__":
    print("Ciphereed Text ", PlayFair().encrypt('helloworld', 'secret'))
    print("Deciphered Text : ", PlayFair().decrypt('JSKYJQEWFQGX', 'secret'))
