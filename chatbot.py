from langchain_ollama import OllamaLLM
from langchain_core.prompts import ChatPromptTemplate

template = """ \
"아래와 같이 답합니다. \
여기 대화 기록이 있습니다: {context}" \
"질문: {question}"
"""

model = OllamaLLM(model = "gemma3:1b")
prompt = ChatPromptTemplate.from_template(template)
chain = prompt | model

def handle_conversation():
    context = ""
    print("환영합니다 AI 챗봇, Type 'exit' to quit.")
    
    while True:
        user_input = input("You: ")
        if user_input.lower() == "exit":
            break
        
        result = chain.invoke({"context": context, "question": user_input})
        print("Bot:", result)
        
        context += f"\nUser: {user_input}\nAI: {result}"

if __name__ == "__main__":
    handle_conversation()
