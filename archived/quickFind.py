# run this to find any text (or matching string in any and all subdir)
import os
import fnmatch

# Function to search for a phrase in a file
def search_in_file(file_path, search_phrase):
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            for line_number, line in enumerate(file, start=1):
                if search_phrase in line:
                    return True, line_number
    except (IOError, UnicodeDecodeError):
        pass
    return False, None

# Function to recursively search for a phrase in files within a directory
def search_phrase_in_directory(directory, search_phrase):
    for root, _, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            found, line_number = search_in_file(file_path, search_phrase)
            if found:
                print(f"Found '{search_phrase}' in '{file_path}' at line {line_number}")

# Main function
if __name__ == "__main__":
    print("\n\nTHIS IS A SHORT AUTOMATION TO FIND ANY AND ALL REFERENCES TO A SPECIFIC STRING \nIN ANY AND ALL SUBDIRECTORIES TO START AT CURRENT DIRECTORY PRESS ENTER\nALTERNATIVELY YOU CAN SPECIFY THE DIRECTORY IN WHICH YOU WOULD LIKE TO SEARCH.")
    directory = input("Enter the directory to search in: ")
    search_phrase = input("Enter the phrase to search for: ")
    if not directory:
       directory="."
    search_phrase_in_directory(directory, search_phrase)
