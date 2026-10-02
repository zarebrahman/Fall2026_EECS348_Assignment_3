/*
 * Program: EECS 348 Assignment 3 - CEO Email Priority Queue
 * Description: Reads email commands from a test file and stores unread emails in a
 *              list-based MaxHeap. Emails are prioritized by sender category first
 *              and newest date second. NEXT displays the highest-priority email,
 *              READ removes it, and COUNT displays the unread-email count.
 * Input: A text file containing EMAIL, NEXT, READ, and COUNT commands.
 * Output: Terminal output showing the next email and/or number of unread emails.
 * Collaborators: None.
 * Other sources: ChatGPT (OpenAI) was used to generate and revise the initial code.
 * Author: Zareb Rahman
 * Creation date: October 1, 2026
 * Revision date: October 1, 2026
 * Revisions: Added object-oriented design, list-based MaxHeap, input parsing,
 *            edge-case handling, and detailed comments.
 */

// Include input/output tools for terminal messages and file reading.
#include <iostream>
// Include file-stream tools so the program can read the test file.
#include <fstream>
// Include strings for sender names, subjects, dates, and input lines.
#include <string>
// Include vectors to provide the list used to store the MaxHeap.
#include <vector>
// Include string streams to split EMAIL lines at commas.
#include <sstream>

// Use standard-library names without writing std:: before each one.
using namespace std;

/*
 * Source note for this block:
 * The Email class was generated with assistance from ChatGPT and reviewed/revised
 * by the author. It stores one email and knows how to calculate its own priority.
 */
class Email {
private:
    // Store the sender category, such as Boss or Peer.
    string sender;
    // Store the email subject line.
    string subject;
    // Store the original date text in MM-DD-YYYY format.
    string date;
    // Store the date as YYYYMMDD so newer dates have larger numbers.
    int dateValue;

    // Convert a date from MM-DD-YYYY into an integer in YYYYMMDD order.
    int convertDate(const string& dateText) const {
        // Read the two-digit month from the start of the string.
        int month = stoi(dateText.substr(0, 2));
        // Read the two-digit day from the middle of the string.
        int day = stoi(dateText.substr(3, 2));
        // Read the four-digit year from the end of the string.
        int year = stoi(dateText.substr(6, 4));
        // Return a number that sorts correctly from oldest to newest.
        return year * 10000 + month * 100 + day;
    }

public:
    // Create an empty email so temporary Email objects can be made when needed.
    Email() : sender(""), subject(""), date(""), dateValue(0) {}

    // Create a complete email from its sender, subject, and date.
    Email(const string& newSender, const string& newSubject, const string& newDate)
        : sender(newSender), subject(newSubject), date(newDate), dateValue(convertDate(newDate)) {}

    // Return the sender category.
    string getSender() const {
        return sender;
    }

    // Return the subject line.
    string getSubject() const {
        return subject;
    }

    // Return the original date string.
    string getDate() const {
        return date;
    }

    // Return the numeric date used for comparisons.
    int getDateValue() const {
        return dateValue;
    }

    // Return the category priority, where a larger number means higher priority.
    int getCategoryPriority() const {
        // Boss emails have the highest category priority.
        if (sender == "Boss") {
            return 5;
        }
        // Subordinate emails are second-highest.
        if (sender == "Subordinate") {
            return 4;
        }
        // Peer emails are third-highest.
        if (sender == "Peer") {
            return 3;
        }
        // ImportantPerson emails are fourth-highest.
        if (sender == "ImportantPerson") {
            return 2;
        }
        // OtherPerson emails have the lowest category priority.
        return 1;
    }
};

/*
 * Source note for this block:
 * The MaxHeap class was generated with assistance from ChatGPT and reviewed/revised
 * by the author. It implements the heap manually with a vector and does not use
 * std::priority_queue, make_heap, push_heap, pop_heap, or another heap module.
 */
class MaxHeap {
private:
    // Store the heap in a list-like vector.
    vector<Email> heap;

    // Return true when the first email should be above the second email in the heap.
    bool higherPriority(const Email& first, const Email& second) const {
        // Compare sender categories before dates.
        if (first.getCategoryPriority() != second.getCategoryPriority()) {
            // The email with the larger category value has higher priority.
            return first.getCategoryPriority() > second.getCategoryPriority();
        }
        // When categories match, the newer date has higher priority.
        return first.getDateValue() > second.getDateValue();
    }

    // Move a newly inserted email upward until the MaxHeap property is restored.
    void heapifyUp(int index) {
        // Keep checking parents while the item is not at the root.
        while (index > 0) {
            // Calculate the parent index in a zero-based list.
            int parent = (index - 1) / 2;
            // Stop when the parent already has equal or higher priority.
            if (!higherPriority(heap[index], heap[parent])) {
                break;
            }
            // Swap the child and parent because the child has higher priority.
            Email temp = heap[index];
            // Move the parent down to the child position.
            heap[index] = heap[parent];
            // Move the higher-priority email up to the parent position.
            heap[parent] = temp;
            // Continue checking from the email's new location.
            index = parent;
        }
    }

    // Move the root downward after a READ removes the highest-priority email.
    void heapifyDown(int index) {
        // Continue until the current item is in the correct position.
        while (true) {
            // Calculate the left child index.
            int left = 2 * index + 1;
            // Calculate the right child index.
            int right = 2 * index + 2;
            // Start by assuming the current item has the highest priority.
            int largest = index;

            // Check whether the left child exists and has higher priority.
            if (left < static_cast<int>(heap.size()) && higherPriority(heap[left], heap[largest])) {
                // Remember that the left child is currently the best choice.
                largest = left;
            }

            // Check whether the right child exists and has higher priority.
            if (right < static_cast<int>(heap.size()) && higherPriority(heap[right], heap[largest])) {
                // Remember that the right child is the best choice.
                largest = right;
            }

            // Stop when neither child has higher priority than the current item.
            if (largest == index) {
                break;
            }

            // Temporarily save the current email before swapping.
            Email temp = heap[index];
            // Move the higher-priority child upward.
            heap[index] = heap[largest];
            // Move the old current email downward.
            heap[largest] = temp;
            // Continue checking from the lower position.
            index = largest;
        }
    }

public:
    // Add one email to the heap.
    void insert(const Email& email) {
        // Put the new email at the end of the list.
        heap.push_back(email);
        // Move it upward if it has more priority than its parent.
        heapifyUp(static_cast<int>(heap.size()) - 1);
    }

    // Return true when no unread emails remain.
    bool empty() const {
        return heap.empty();
    }

    // Return the number of unread emails.
    int size() const {
        return static_cast<int>(heap.size());
    }

    // Return the highest-priority email without removing it.
    const Email& peek() const {
        return heap[0];
    }

    // Remove the highest-priority email if one exists.
    void removeMax() {
        // Do nothing if there are no unread emails.
        if (heap.empty()) {
            return;
        }

        // Replace the root with the last email in the list.
        heap[0] = heap.back();
        // Remove the old last position from the list.
        heap.pop_back();

        // Restore the heap only when at least one email remains.
        if (!heap.empty()) {
            // Move the new root downward to its correct position.
            heapifyDown(0);
        }
    }
};

/*
 * Source note for this block:
 * The EmailProgram class was generated with assistance from ChatGPT and revised by
 * the author. It handles all file parsing and command processing through objects.
 */
class EmailProgram {
private:
    // Store all unread emails inside the custom MaxHeap object.
    MaxHeap inbox;

    // Process one EMAIL command and add the parsed email to the heap.
    void processEmail(const string& line) {
        // Remove the "EMAIL " part from the beginning of the command.
        string data = line.substr(6);
        // Create a stream that can split the remaining text by commas.
        stringstream ss(data);
        // Create a variable for the sender category.
        string sender;
        // Create a variable for the subject line.
        string subject;
        // Create a variable for the date.
        string date;

        // Read everything before the first comma as the sender category.
        getline(ss, sender, ',');
        // Read everything before the second comma as the subject line.
        getline(ss, subject, ',');
        // Read the rest of the line as the date.
        getline(ss, date);

        // Create an Email object from the three parsed values.
        Email email(sender, subject, date);
        // Add the Email object to the custom MaxHeap.
        inbox.insert(email);
    }

    // Process a NEXT command without removing the email.
    void processNext() const {
        // Handle NEXT safely when the inbox is empty.
        if (inbox.empty()) {
            // Tell the user that there is no email available to display.
            cout << "No emails to read." << endl;
            // End this command without accessing the empty heap.
            return;
        }

        // Get the highest-priority email without removing it.
        const Email& email = inbox.peek();
        // Print the heading required by the sample output.
        cout << "Next email:" << endl;
        // Print the sender category.
        cout << "Sender: " << email.getSender() << endl;
        // Print the subject line.
        cout << "Subject: " << email.getSubject() << endl;
        // Print the date.
        cout << "Date: " << email.getDate() << endl;
    }

    // Process a READ command by removing the highest-priority email.
    void processRead() {
        // removeMax safely does nothing when the heap is empty.
        inbox.removeMax();
    }

    // Process a COUNT command by displaying the number of unread emails.
    void processCount() const {
        // Print the unread-email count in the format shown in the assignment sample.
        cout << "There are " << inbox.size() << " emails to read." << endl;
    }

public:
    // Read and process every command in the given test file.
    bool run(const string& fileName) {
        // Open the input file for reading.
        ifstream inputFile(fileName);

        // Check whether the file opened successfully.
        if (!inputFile.is_open()) {
            // Print an error message if the file could not be opened.
            cerr << "Error: Could not open " << fileName << endl;
            // Tell main that the program failed.
            return false;
        }

        // Store one input line at a time.
        string line;
        // Continue until every line in the file has been processed.
        while (getline(inputFile, line)) {
            // Ignore completely empty lines.
            if (line.empty()) {
                continue;
            }

            // Check whether this line is an EMAIL command.
            if (line.rfind("EMAIL ", 0) == 0) {
                // Parse and insert the email.
                processEmail(line);
            }
            // Check whether this line is a NEXT command.
            else if (line == "NEXT") {
                // Display the current highest-priority email without removing it.
                processNext();
            }
            // Check whether this line is a READ command.
            else if (line == "READ") {
                // Remove the current highest-priority email without displaying it.
                processRead();
            }
            // Check whether this line is a COUNT command.
            else if (line == "COUNT") {
                // Display the number of unread emails.
                processCount();
            }
        }

        // Close the file after all commands have been processed.
        inputFile.close();
        // Tell main that the program completed successfully.
        return true;
    }
};

/*
 * Source note for this block:
 * main was generated with assistance from ChatGPT and reviewed by the author.
 * It creates the program object and starts processing the selected test file.
 */
int main(int argc, char* argv[]) {
    // Require the test-file name as a command-line argument.
    if (argc != 2) {
        // Show the expected command format when the file name is missing.
        cerr << "Usage: " << argv[0] << " <test_file>" << endl;
        // Exit with an error status.
        return 1;
    }

    // Create the object that controls the email-priority program.
    EmailProgram program;
    // Run the program using the file name supplied on the command line.
    if (!program.run(argv[1])) {
        // Exit with an error status if the file could not be processed.
        return 1;
    }

    // Exit successfully after the entire test file is complete.
    return 0;
}
