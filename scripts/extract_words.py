import re

with open("../war_and_peace.txt", "r", encoding="utf-8") as f:
    text = f.read()

words = re.findall(r"[a-zA-Z']+", text)
unique_words = sorted(set(w.lower() for w in words))

with open("../words.txt", "w") as f:
    f.write("\n".join(unique_words))

print(f"Unique words: {len(unique_words)}")
