#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Structure for Medicine
typedef struct {
    int id;
    char name[100];
    float price;
    int quantity;
    char category[50];
    char expiry_date[20];
} Medicine;

// Structure for Cart Item
typedef struct CartItem {
    int medicine_id;
    char medicine_name[100];
    float price;
    int quantity;
    struct CartItem* next;
} CartItem;

// Structure for Cart
typedef struct {
    CartItem* items;
    int item_count;
    float subtotal;
    float tax;
    float total;
} Cart;

// Structure for Transaction Item
typedef struct {
    int medicine_id;
    char medicine_name[100];
    float price;
    int quantity;
} TransactionItem;

// Structure for Transaction
typedef struct {
    int transaction_id;
    char date[20];
    char time[20];
    float amount;
    int items_count;
    TransactionItem items[100]; // Store details of purchased items
} Transaction;

// Global variables
#define MAX_MEDICINES 1000
#define MEDICINE_FILE "medicines.dat"
#define TRANSACTION_BIN_FILE "transactions.dat"
#define TRANSACTION_TEXT_FILE "transactions.txt"
#define ADMIN_PASSWORD "admin123"

// Function prototypes
void displayMainMenu();
void adminPanel();
void customerPanel();
int authenticateAdmin();
void addMedicine();
void viewMedicines();
void searchMedicine();
void updateMedicine();
void deleteMedicine();
void viewLowStock();
void browseMedicines();
void addToCart(Cart* cart);
void removeFromCart(Cart* cart);
void viewCart(Cart* cart);
void checkout(Cart* cart);
void processPayment(Cart* cart);
void saveTransactionToBinary(Transaction* trans);
void saveTransactionToText(Transaction* trans);
void viewTransactions();
void viewTransactionsFromText();
void loadMedicines(Medicine medicines[], int* count);
void saveMedicines(Medicine medicines[], int count);
int generateMedicineId();
int generateTransactionId();
void clearInputBuffer();
void printHeader(const char* title);
void printLine(char ch, int length);

int main() {
    printf("\n");
    printLine('=', 60);
    printf("    MEDICAL STORE MANAGEMENT SYSTEM\n");
    printLine('=', 60);
    
    int choice;
    
    do {
        displayMainMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                adminPanel();
                break;
            case 2:
                customerPanel();
                break;
            case 3:
                printf("\nThank you for using Medical Store Management System!\n");
                break;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    } while(choice != 3);
    
    return 0;
}

void displayMainMenu() {
    printf("\n");
    printLine('-', 40);
    printf("          MAIN MENU\n");
    printLine('-', 40);
    printf("1. Admin Panel\n");
    printf("2. Customer Panel\n");
    printf("3. Exit\n");
}

void adminPanel() {
    if (!authenticateAdmin()) {
        printf("\nAccess denied! Invalid password.\n");
        return;
    }
    
    int choice;
    
    do {
        printf("\n");
        printLine('-', 40);
        printf("          ADMIN PANEL\n");
        printLine('-', 40);
        printf("1. Add New Medicine\n");
        printf("2. View All Medicines\n");
        printf("3. Search Medicine\n");
        printf("4. Update Medicine\n");
        printf("5. Delete Medicine\n");
        printf("6. View Low Stock Medicines\n");
        printf("7. View Transactions (Binary)\n");
        printf("8. View Transactions (Text File)\n");
        printf("9. Return to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                addMedicine();
                break;
            case 2:
                viewMedicines();
                break;
            case 3:
                searchMedicine();
                break;
            case 4:
                updateMedicine();
                break;
            case 5:
                deleteMedicine();
                break;
            case 6:
                viewLowStock();
                break;
            case 7:
                viewTransactions();
                break;
            case 8:
                viewTransactionsFromText();
                break;
            case 9:
                printf("\nReturning to Main Menu...\n");
                break;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    } while(choice != 9);
}

int authenticateAdmin() {
    char password[50];
    printf("\nEnter admin password: ");
    scanf("%s", password);
    clearInputBuffer();
    return (strcmp(password, ADMIN_PASSWORD) == 0);
}

void addMedicine() {
    printHeader("ADD NEW MEDICINE");
    
    Medicine med;
    Medicine medicines[MAX_MEDICINES];
    int count = 0;
    
    loadMedicines(medicines, &count);
    
    if (count >= MAX_MEDICINES) {
        printf("Medicine database is full!\n");
        return;
    }
    
    med.id = generateMedicineId();
    
    printf("Enter medicine name: ");
    fgets(med.name, sizeof(med.name), stdin);
    med.name[strcspn(med.name, "\n")] = 0;
    
    printf("Enter category (e.g., Tablet, Syrup, Injection): ");
    fgets(med.category, sizeof(med.category), stdin);
    med.category[strcspn(med.category, "\n")] = 0;
    
    printf("Enter price: ");
    scanf("%f", &med.price);
    
    printf("Enter quantity: ");
    scanf("%d", &med.quantity);
    clearInputBuffer();
    
    printf("Enter expiry date (DD/MM/YYYY): ");
    fgets(med.expiry_date, sizeof(med.expiry_date), stdin);
    med.expiry_date[strcspn(med.expiry_date, "\n")] = 0;
    
    medicines[count] = med;
    count++;
    
    saveMedicines(medicines, count);
    
    printf("\nMedicine added successfully!\n");
    printf("Medicine ID: %d\n", med.id);
}

void viewMedicines() {
    printHeader("ALL MEDICINES INVENTORY");
    
    Medicine medicines[MAX_MEDICINES];
    int count = 0;
    
    loadMedicines(medicines, &count);
    
    if (count == 0) {
        printf("No medicines found in inventory.\n");
        return;
    }
    
    printf("%-10s %-30s %-20s %-10s %-8s %-12s\n", 
           "ID", "Name", "Category", "Price", "Qty", "Expiry");
    printLine('-', 100);
    
    float total_value = 0;
    for (int i = 0; i < count; i++) {
        printf("%-10d %-30s %-20s %-10.2f %-8d %-12s\n",
               medicines[i].id,
               medicines[i].name,
               medicines[i].category,
               medicines[i].price,
               medicines[i].quantity,
               medicines[i].expiry_date);
        total_value += medicines[i].price * medicines[i].quantity;
    }
    
    printLine('-', 100);
    printf("Total Medicines: %d\n", count);
    printf("Total Inventory Value: $%.2f\n", total_value);
}

void searchMedicine() {
    printHeader("SEARCH MEDICINE");
    
    Medicine medicines[MAX_MEDICINES];
    int count = 0;
    char search_term[100];
    int found = 0;
    
    loadMedicines(medicines, &count);
    
    printf("Enter medicine name or ID to search: ");
    fgets(search_term, sizeof(search_term), stdin);
    search_term[strcspn(search_term, "\n")] = 0;
    
    printf("\n%-10s %-30s %-20s %-10s %-8s %-12s\n", 
           "ID", "Name", "Category", "Price", "Qty", "Expiry");
    printLine('-', 100);
    
    for (int i = 0; i < count; i++) {
        char id_str[20];
        sprintf(id_str, "%d", medicines[i].id);
        
        if (strstr(medicines[i].name, search_term) != NULL || 
            strcmp(id_str, search_term) == 0) {
            printf("%-10d %-30s %-20s %-10.2f %-8d %-12s\n",
                   medicines[i].id,
                   medicines[i].name,
                   medicines[i].category,
                   medicines[i].price,
                   medicines[i].quantity,
                   medicines[i].expiry_date);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No medicines found matching '%s'\n", search_term);
    }
}

void updateMedicine() {
    printHeader("UPDATE MEDICINE");
    
    Medicine medicines[MAX_MEDICINES];
    int count = 0;
    int id, found = 0;
    
    loadMedicines(medicines, &count);
    
    printf("Enter Medicine ID to update: ");
    scanf("%d", &id);
    clearInputBuffer();
    
    for (int i = 0; i < count; i++) {
        if (medicines[i].id == id) {
            found = 1;
            
            printf("\nCurrent Details:\n");
            printf("Name: %s\n", medicines[i].name);
            printf("Category: %s\n", medicines[i].category);
            printf("Price: %.2f\n", medicines[i].price);
            printf("Quantity: %d\n", medicines[i].quantity);
            printf("Expiry: %s\n", medicines[i].expiry_date);
            
            printf("\nEnter new details (press Enter to keep current value):\n");
            
            char input[100];
            
            printf("Name [%s]: ", medicines[i].name);
            fgets(input, sizeof(input), stdin);
            if (strlen(input) > 1) {
                input[strcspn(input, "\n")] = 0;
                strcpy(medicines[i].name, input);
            }
            
            printf("Category [%s]: ", medicines[i].category);
            fgets(input, sizeof(input), stdin);
            if (strlen(input) > 1) {
                input[strcspn(input, "\n")] = 0;
                strcpy(medicines[i].category, input);
            }
            
            printf("Price [%.2f]: ", medicines[i].price);
            fgets(input, sizeof(input), stdin);
            if (strlen(input) > 1) {
                medicines[i].price = atof(input);
            }
            
            printf("Quantity [%d]: ", medicines[i].quantity);
            fgets(input, sizeof(input), stdin);
            if (strlen(input) > 1) {
                medicines[i].quantity = atoi(input);
            }
            
            printf("Expiry Date [%s]: ", medicines[i].expiry_date);
            fgets(input, sizeof(input), stdin);
            if (strlen(input) > 1) {
                input[strcspn(input, "\n")] = 0;
                strcpy(medicines[i].expiry_date, input);
            }
            
            saveMedicines(medicines, count);
            printf("\nMedicine updated successfully!\n");
            break;
        }
    }
    
    if (!found) {
        printf("Medicine with ID %d not found!\n", id);
    }
}

void deleteMedicine() {
    printHeader("DELETE MEDICINE");
    
    Medicine medicines[MAX_MEDICINES];
    int count = 0;
    int id, found = 0;
    
    loadMedicines(medicines, &count);
    
    printf("Enter Medicine ID to delete: ");
    scanf("%d", &id);
    clearInputBuffer();
    
    for (int i = 0; i < count; i++) {
        if (medicines[i].id == id) {
            found = 1;
            
            printf("\nMedicine to delete:\n");
            printf("ID: %d\n", medicines[i].id);
            printf("Name: %s\n", medicines[i].name);
            printf("Price: %.2f\n", medicines[i].price);
            printf("Quantity: %d\n", medicines[i].quantity);
            
            char confirm;
            printf("\nAre you sure you want to delete this medicine? (y/n): ");
            scanf("%c", &confirm);
            clearInputBuffer();
            
            if (confirm == 'y' || confirm == 'Y') {
                // Shift all elements after i one position left
                for (int j = i; j < count - 1; j++) {
                    medicines[j] = medicines[j + 1];
                }
                count--;
                
                saveMedicines(medicines, count);
                printf("Medicine deleted successfully!\n");
            } else {
                printf("Deletion cancelled.\n");
            }
            break;
        }
    }
    
    if (!found) {
        printf("Medicine with ID %d not found!\n", id);
    }
}

void viewLowStock() {
    printHeader("LOW STOCK MEDICINES (Quantity < 10)");
    
    Medicine medicines[MAX_MEDICINES];
    int count = 0;
    int found = 0;
    
    loadMedicines(medicines, &count);
    
    printf("%-10s %-30s %-20s %-10s %-8s %-12s\n", 
           "ID", "Name", "Category", "Price", "Qty", "Expiry");
    printLine('-', 100);
    
    for (int i = 0; i < count; i++) {
        if (medicines[i].quantity < 10) {
            printf("%-10d %-30s %-20s %-10.2f %-8d %-12s\n",
                   medicines[i].id,
                   medicines[i].name,
                   medicines[i].category,
                   medicines[i].price,
                   medicines[i].quantity,
                   medicines[i].expiry_date);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No low stock medicines found.\n");
    }
}

void customerPanel() {
    Cart cart;
    cart.items = NULL;
    cart.item_count = 0;
    cart.subtotal = 0;
    cart.tax = 0;
    cart.total = 0;
    
    int choice;
    
    do {
        printf("\n");
        printLine('-', 40);
        printf("        CUSTOMER PANEL\n");
        printLine('-', 40);
        printf("1. Browse Medicines\n");
        printf("2. Search Medicine\n");
        printf("3. View Cart\n");
        printf("4. Remove from Cart\n");
        printf("5. Checkout\n");
        printf("6. Return to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                browseMedicines();
                addToCart(&cart);
                break;
            case 2:
                searchMedicine();
                addToCart(&cart);
                break;
            case 3:
                viewCart(&cart);
                break;
            case 4:
                removeFromCart(&cart);
                break;
            case 5:
                if (cart.item_count > 0) {
                    checkout(&cart);
                } else {
                    printf("\nYour cart is empty!\n");
                }
                break;
            case 6:
                // Free cart memory
                CartItem* current = cart.items;
                while (current != NULL) {
                    CartItem* temp = current;
                    current = current->next;
                    free(temp);
                }
                printf("\nReturning to Main Menu...\n");
                break;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    } while(choice != 6);
}

void browseMedicines() {
    printHeader("BROWSE MEDICINES");
    
    Medicine medicines[MAX_MEDICINES];
    int count = 0;
    
    loadMedicines(medicines, &count);
    
    if (count == 0) {
        printf("No medicines available.\n");
        return;
    }
    
    // Group by category
    char categories[MAX_MEDICINES][50];
    int cat_count = 0;
    
    // Collect unique categories
    for (int i = 0; i < count; i++) {
        int found = 0;
        for (int j = 0; j < cat_count; j++) {
            if (strcmp(categories[j], medicines[i].category) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(categories[cat_count], medicines[i].category);
            cat_count++;
        }
    }
    
    // Display by category
    for (int c = 0; c < cat_count; c++) {
        printf("\n%s:\n", categories[c]);
        printf("%-5s %-30s %-10s %-8s\n", "ID", "Name", "Price", "Stock");
        printLine('-', 60);
        
        for (int i = 0; i < count; i++) {
            if (strcmp(medicines[i].category, categories[c]) == 0 && medicines[i].quantity > 0) {
                printf("%-5d %-30s %-10.2f %-8d\n",
                       medicines[i].id,
                       medicines[i].name,
                       medicines[i].price,
                       medicines[i].quantity);
            }
        }
    }
}

void addToCart(Cart* cart) {
    Medicine medicines[MAX_MEDICINES];
    int count = 0;
    int id, quantity;
    
    loadMedicines(medicines, &count);
    
    printf("\nEnter Medicine ID to add to cart (0 to skip): ");
    scanf("%d", &id);
    
    if (id == 0) {
        clearInputBuffer();
        return;
    }
    
    printf("Enter quantity: ");
    scanf("%d", &quantity);
    clearInputBuffer();
    
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (medicines[i].id == id) {
            found = 1;
            
            if (quantity <= 0) {
                printf("Invalid quantity!\n");
                return;
            }
            
            if (quantity > medicines[i].quantity) {
                printf("Insufficient stock! Available: %d\n", medicines[i].quantity);
                return;
            }
            
            // Check if already in cart
            CartItem* current = cart->items;
            while (current != NULL) {
                if (current->medicine_id == id) {
                    current->quantity += quantity;
                    printf("Quantity updated in cart!\n");
                    return;
                }
                current = current->next;
            }
            
            // Add new item to cart
            CartItem* new_item = (CartItem*)malloc(sizeof(CartItem));
            new_item->medicine_id = id;
            strcpy(new_item->medicine_name, medicines[i].name);
            new_item->price = medicines[i].price;
            new_item->quantity = quantity;
            new_item->next = cart->items;
            cart->items = new_item;
            cart->item_count++;
            
            printf("Added to cart: %s x %d\n", medicines[i].name, quantity);
            break;
        }
    }
    
    if (!found) {
        printf("Medicine with ID %d not found!\n", id);
    }
}

void removeFromCart(Cart* cart) {
    if (cart->item_count == 0) {
        printf("\nYour cart is empty!\n");
        return;
    }
    
    viewCart(cart);
    
    int id;
    printf("\nEnter Medicine ID to remove from cart (0 to cancel): ");
    scanf("%d", &id);
    clearInputBuffer();
    
    if (id == 0) {
        return;
    }
    
    CartItem* current = cart->items;
    CartItem* prev = NULL;
    int found = 0;
    
    while (current != NULL) {
        if (current->medicine_id == id) {
            found = 1;
            
            printf("Found: %s (Quantity: %d)\n", current->medicine_name, current->quantity);
            printf("Enter quantity to remove (0 to remove all): ");
            int remove_qty;
            scanf("%d", &remove_qty);
            clearInputBuffer();
            
            if (remove_qty <= 0 || remove_qty >= current->quantity) {
                // Remove entire item
                if (prev == NULL) {
                    cart->items = current->next;
                } else {
                    prev->next = current->next;
                }
                
                printf("Removed %s from cart.\n", current->medicine_name);
                free(current);
                cart->item_count--;
            } else {
                // Reduce quantity
                current->quantity -= remove_qty;
                printf("Reduced quantity of %s by %d. Remaining: %d\n", 
                       current->medicine_name, remove_qty, current->quantity);
            }
            break;
        }
        prev = current;
        current = current->next;
    }
    
    if (!found) {
        printf("Medicine with ID %d not found in cart!\n", id);
    }
}

void viewCart(Cart* cart) {
    printHeader("YOUR SHOPPING CART");
    
    if (cart->item_count == 0) {
        printf("Your cart is empty.\n");
        return;
    }
    
    printf("%-5s %-30s %-10s %-8s %-10s\n", 
           "ID", "Name", "Price", "Qty", "Total");
    printLine('-', 73);
    
    float subtotal = 0;
    CartItem* current = cart->items;
    while (current != NULL) {
        float item_total = current->price * current->quantity;
        printf("%-5d %-30s %-10.2f %-8d %-10.2f\n",
               current->medicine_id,
               current->medicine_name,
               current->price,
               current->quantity,
               item_total);
        subtotal += item_total;
        current = current->next;
    }
    
    printLine('-', 73);
    float tax = subtotal * 0.08; // 8% tax
    float total = subtotal + tax;
    
    printf("Subtotal: $%.2f\n", subtotal);
    printf("Tax (8%%): $%.2f\n", tax);
    printf("Total: $%.2f\n", total);
    
    cart->subtotal = subtotal;
    cart->tax = tax;
    cart->total = total;
}

void checkout(Cart* cart) {
    printHeader("CHECKOUT");
    
    viewCart(cart);
    
    if (cart->item_count == 0) {
        return;
    }
    
    printf("\nProceed to payment? (y/n): ");
    char confirm;
    scanf("%c", &confirm);
    clearInputBuffer();
    
    if (confirm == 'y' || confirm == 'Y') {
        processPayment(cart);
    } else {
        printf("Checkout cancelled.\n");
    }
}

void processPayment(Cart* cart) {
    printHeader("PAYMENT PROCESSING");
    
    printf("Total Amount Due: $%.2f\n", cart->total);
    
    float amount_paid;
    printf("Enter amount paid: $");
    scanf("%f", &amount_paid);
    clearInputBuffer();
    
    if (amount_paid < cart->total) {
        printf("Insufficient payment! Transaction cancelled.\n");
        return;
    }
    
    float change = amount_paid - cart->total;
    printf("Payment successful!\n");
    printf("Change: $%.2f\n", change);
    
    // Update inventory and prepare transaction data
    Medicine medicines[MAX_MEDICINES];
    int count = 0;
    
    loadMedicines(medicines, &count);
    
    // Create transaction record with details
    Transaction trans;
    trans.transaction_id = generateTransactionId();
    trans.amount = cart->total;
    trans.items_count = 0;
    
    // Get current date and time
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    strftime(trans.date, sizeof(trans.date), "%d/%m/%Y", tm_info);
    strftime(trans.time, sizeof(trans.time), "%H:%M:%S", tm_info);
    
    CartItem* current = cart->items;
    int items_sold = 0;
    
    while (current != NULL) {
        // Update inventory
        for (int i = 0; i < count; i++) {
            if (medicines[i].id == current->medicine_id) {
                medicines[i].quantity -= current->quantity;
                items_sold += current->quantity;
                
                // Add to transaction details
                trans.items[trans.items_count].medicine_id = current->medicine_id;
                strcpy(trans.items[trans.items_count].medicine_name, current->medicine_name);
                trans.items[trans.items_count].price = current->price;
                trans.items[trans.items_count].quantity = current->quantity;
                trans.items_count++;
                break;
            }
        }
        current = current->next;
    }
    
    saveMedicines(medicines, count);
    saveTransactionToBinary(&trans);
    saveTransactionToText(&trans);
    
    // Generate receipt
    printf("\n");
    printLine('=', 50);
    printf("          SALES RECEIPT\n");
    printLine('=', 50);
    printf("Transaction ID: %d\n", trans.transaction_id);
    printf("Date: %s\n", trans.date);
    printf("Time: %s\n", trans.time);
    printf("\nItems Purchased:\n");
    printf("%-30s %-8s %-10s %-10s\n", "Medicine", "Qty", "Price", "Total");
    printLine('-', 58);
    
    for (int i = 0; i < trans.items_count; i++) {
        printf("%-30s %-8d $%-9.2f $%-9.2f\n",
               trans.items[i].medicine_name,
               trans.items[i].quantity,
               trans.items[i].price,
               trans.items[i].price * trans.items[i].quantity);
    }
    
    printLine('-', 58);
    printf("Subtotal: $%.2f\n", cart->subtotal);
    printf("Tax (8%%): $%.2f\n", cart->tax);
    printf("Total: $%.2f\n", cart->total);
    printf("Paid: $%.2f\n", amount_paid);
    printf("Change: $%.2f\n", change);
    printLine('=', 50);
    printf("Thank you for your purchase!\n");
    printf("Transaction saved to: %s\n", TRANSACTION_TEXT_FILE);
    
    // Clear cart
    current = cart->items;
    while (current != NULL) {
        CartItem* temp = current;
        current = current->next;
        free(temp);
    }
    
    cart->items = NULL;
    cart->item_count = 0;
    cart->subtotal = 0;
    cart->tax = 0;
    cart->total = 0;
}

void saveTransactionToBinary(Transaction* trans) {
    FILE* file = fopen(TRANSACTION_BIN_FILE, "ab");
    if (file == NULL) {
        printf("Error saving transaction to binary file!\n");
        return;
    }
    
    fwrite(trans, sizeof(Transaction), 1, file);
    fclose(file);
}

void saveTransactionToText(Transaction* trans) {
    FILE* file = fopen(TRANSACTION_TEXT_FILE, "a");
    if (file == NULL) {
        printf("Error saving transaction to text file!\n");
        return;
    }
    
    fprintf(file, "\n========================================\n");
    fprintf(file, "TRANSACTION ID: %d\n", trans->transaction_id);
    fprintf(file, "Date: %s | Time: %s\n", trans->date, trans->time);
    fprintf(file, "----------------------------------------\n");
    fprintf(file, "ITEMS PURCHASED:\n");
    fprintf(file, "%-30s %-8s %-10s %-10s\n", "Medicine", "Qty", "Price", "Total");
    fprintf(file, "----------------------------------------\n");
    
    for (int i = 0; i < trans->items_count; i++) {
        fprintf(file, "%-30s %-8d $%-9.2f $%-9.2f\n",
                trans->items[i].medicine_name,
                trans->items[i].quantity,
                trans->items[i].price,
                trans->items[i].price * trans->items[i].quantity);
    }
    
    fprintf(file, "----------------------------------------\n");
    fprintf(file, "Total Items: %d\n", trans->items_count);
    fprintf(file, "Total Amount: $%.2f\n", trans->amount);
    fprintf(file, "========================================\n\n");
    
    fclose(file);
    printf("Transaction details saved to %s\n", TRANSACTION_TEXT_FILE);
}

void viewTransactions() {
    printHeader("TRANSACTION HISTORY (Binary File)");
    
    FILE* file = fopen(TRANSACTION_BIN_FILE, "rb");
    if (file == NULL) {
        printf("No transactions found.\n");
        return;
    }
    
    printf("%-15s %-12s %-10s %-10s %-10s\n", 
           "Transaction ID", "Date", "Time", "Items", "Amount");
    printLine('-', 60);
    
    Transaction trans;
    float total_sales = 0;
    int total_transactions = 0;
    
    while (fread(&trans, sizeof(Transaction), 1, file)) {
        printf("%-15d %-12s %-10s %-10d $%-9.2f\n",
               trans.transaction_id,
               trans.date,
               trans.time,
               trans.items_count,
               trans.amount);
        total_sales += trans.amount;
        total_transactions++;
    }
    
    fclose(file);
    
    printLine('-', 60);
    printf("Total Transactions: %d\n", total_transactions);
    printf("Total Sales: $%.2f\n", total_sales);
}

void viewTransactionsFromText() {
    printHeader("TRANSACTION HISTORY (Text File)");
    
    FILE* file = fopen(TRANSACTION_TEXT_FILE, "r");
    if (file == NULL) {
        printf("No transaction text file found.\n");
        return;
    }
    
    char line[200];
    printf("\nContents of %s:\n", TRANSACTION_TEXT_FILE);
    printLine('-', 60);
    
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    
    fclose(file);
    printLine('-', 60);
}

void loadMedicines(Medicine medicines[], int* count) {
    FILE* file = fopen(MEDICINE_FILE, "rb");
    *count = 0;
    
    if (file == NULL) {
        return;
    }
    
    while (fread(&medicines[*count], sizeof(Medicine), 1, file)) {
        (*count)++;
        if (*count >= MAX_MEDICINES) {
            break;
        }
    }
    
    fclose(file);
}

void saveMedicines(Medicine medicines[], int count) {
    FILE* file = fopen(MEDICINE_FILE, "wb");
    if (file == NULL) {
        printf("Error saving medicines!\n");
        return;
    }
    
    fwrite(medicines, sizeof(Medicine), count, file);
    fclose(file);
}

int generateMedicineId() {
    static int last_id = 1000;
    last_id++;
    return last_id;
}

int generateTransactionId() {
    static int last_trans_id = 5000;
    last_trans_id++;
    return last_trans_id;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printHeader(const char* title) {
    printf("\n");
    printLine('=', 50);
    printf("          %s\n", title);
    printLine('=', 50);
}

void printLine(char ch, int length) {
    for (int i = 0; i < length; i++) {
        printf("%c", ch);
    }
    printf("\n");
}
