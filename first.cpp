/*
  Medical Store Management System (C)
  - Admin: login, add, view, search, update, delete medicines
  - Staff: search, view inventory
  - Customer: browse, search, add/remove cart, checkout
  - Billing: VAT included, configurable TAX_RATE
  - Persistence: medicines.dat (binary), sales_history.txt (text append)
  - Customer name at checkout is optional (press Enter to skip)
  - Compile: gcc -o medstore medstore.c
  - Run: ./medstore
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define DATAFILE "medicines.dat"
#define SALESFILE "sales_history.txt"
#define NAME_LEN 64
#define ADMIN_PASS "admin123"
#define TAX_RATE 0.05   /* 5% VAT (adjust if needed) */
#define MAX_CART 100

/* Medicine record */
typedef struct {
    int id;
    char name[NAME_LEN];
    double price;
    int quantity;
    int expiry_day;
    int expiry_month;
    int expiry_year;
} Medicine;

/* Cart item */
typedef struct {
    int med_id;
    char name[NAME_LEN];
    double price;
    int qty;
} CartItem;

/* Utility to pause */
void pressEnterToContinue() {
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

/* Convert string to lowercase copy */
void strtolower_copy(const char *src, char *dst, size_t dstlen) {
    size_t i;
    for (i = 0; i + 1 < dstlen && src[i]; ++i) dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

/* Case-insensitive substring check */
int ci_substr(const char *haystack, const char *needle) {
    char h[512], n[512];
    strncpy(h, haystack, sizeof(h)-1); h[sizeof(h)-1] = '\0';
    strncpy(n, needle, sizeof(n)-1); n[sizeof(n)-1] = '\0';
    strtolower_copy(h, h, sizeof(h));
    strtolower_copy(n, n, sizeof(n));
    return strstr(h, n) != NULL;
}

/* Get next medicine ID (max ID + 1) */
int getNextMedicineID() {
    FILE *fp = fopen(DATAFILE, "rb");
    if (!fp) return 1;
    Medicine m;
    int max_id = 0;
    while (fread(&m, sizeof(Medicine), 1, fp) == 1) {
        if (m.id > max_id) max_id = m.id;
    }
    fclose(fp);
    return max_id + 1;
}

/* Add a new medicine */
void addMedicine() {
    Medicine m;
    m.id = getNextMedicineID();

    printf("\n--- Add New Medicine ---\n");
    printf("Name: ");
    getchar(); /* clear newline */
    fgets(m.name, NAME_LEN, stdin);
    m.name[strcspn(m.name, "\n")] = '\0';

    printf("Price: ");
    if (scanf("%lf", &m.price) != 1) { printf("Invalid input.\n"); while(getchar()!='\n'); return; }
    printf("Quantity: ");
    if (scanf("%d", &m.quantity) != 1) { printf("Invalid input.\n"); while(getchar()!='\n'); return; }
    printf("Expiry Day (1-31): "); scanf("%d", &m.expiry_day);
    printf("Expiry Month (1-12): "); scanf("%d", &m.expiry_month);
    printf("Expiry Year (e.g., 2026): "); scanf("%d", &m.expiry_year);

    FILE *fp = fopen(DATAFILE, "ab");
    if (!fp) { perror("Unable to open data file"); return; }
    fwrite(&m, sizeof(Medicine), 1, fp);
    fclose(fp);

    printf("\nMedicine added with ID: %d\n", m.id);
}

/* Print a medicine (single) */
void printMedicine(const Medicine *m) {
    printf("ID: %d | %s | Price: %.2f | Qty: %d | Exp: %02d-%02d-%04d\n",
           m->id, m->name, m->price, m->quantity,
           m->expiry_day, m->expiry_month, m->expiry_year);
}

/* View all medicines */
void viewMedicines() {
    FILE *fp = fopen(DATAFILE, "rb");
    if (!fp) { printf("\nNo medicines available.\n"); return; }
    Medicine m;
    printf("\n--- Medicine List ---\n");
    int found = 0;
    while (fread(&m, sizeof(Medicine), 1, fp) == 1) {
        printMedicine(&m);
        found = 1;
    }
    if (!found) printf("No medicines in inventory.\n");
    fclose(fp);
}

/* Search medicine by exact id, returns 1 and fills out if found */
int searchMedicineByID(int id, Medicine *out) {
    FILE *fp = fopen(DATAFILE, "rb");
    if (!fp) return 0;
    Medicine m;
    while (fread(&m, sizeof(Medicine), 1, fp) == 1) {
        if (m.id == id) {
            if (out) *out = m;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Search medicine by name (partial, case-insensitive) - prints matches */
int searchMedicineByName(const char *name) {
    FILE *fp = fopen(DATAFILE, "rb");
    if (!fp) { printf("\nNo medicines available.\n"); return 0; }
    Medicine m;
    int found = 0;
    printf("\nSearch results for \"%s\":\n", name);
    while (fread(&m, sizeof(Medicine), 1, fp) == 1) {
        if (ci_substr(m.name, name)) {
            printMedicine(&m);
            found = 1;
        }
    }
    if (!found) printf("No matches found.\n");
    fclose(fp);
    return found;
}

/* Update medicine (by id) */
void updateMedicine() {
    printf("\n--- Update Medicine ---\n");
    printf("Enter medicine ID: ");
    int id; if (scanf("%d", &id) != 1) { printf("Invalid input.\n"); while(getchar()!='\n'); return; }

    FILE *fp = fopen(DATAFILE, "rb+");
    if (!fp) { printf("No data file.\n"); return; }

    Medicine m;
    int found = 0;
    while (fread(&m, sizeof(Medicine), 1, fp) == 1) {
        if (m.id == id) {
            found = 1;
            printf("Existing record:\n"); printMedicine(&m);
            getchar(); /* consume newline */
            printf("New Name (leave blank to keep): ");
            char newname[NAME_LEN]; fgets(newname, NAME_LEN, stdin);
            if (newname[0] != '\n') {
                newname[strcspn(newname, "\n")] = '\0';
                strncpy(m.name, newname, NAME_LEN);
            }
            printf("New Price (-1 to keep %.2f): ", m.price);
            double newprice; if (scanf("%lf", &newprice) == 1 && newprice >= 0) m.price = newprice;
            printf("New Quantity (-1 to keep %d): ", m.quantity);
            int newqty; if (scanf("%d", &newqty) == 1 && newqty >= 0) m.quantity = newqty;
            printf("New Expiry Day (0 to keep %d): ", m.expiry_day); int nd; if (scanf("%d", &nd) == 1 && nd>0) m.expiry_day = nd;
            printf("New Expiry Month (0 to keep %d): ", m.expiry_month); int nm; if (scanf("%d", &nm) == 1 && nm>0) m.expiry_month = nm;
            printf("New Expiry Year (0 to keep %d): ", m.expiry_year); int ny; if (scanf("%d", &ny) == 1 && ny>0) m.expiry_year = ny;

            /* move file pointer back to overwrite */
            fseek(fp, - (long)sizeof(Medicine), SEEK_CUR);
            fwrite(&m, sizeof(Medicine), 1, fp);
            printf("Record updated.\n");
            break;
        }
    }
    if (!found) printf("Medicine with ID %d not found.\n", id);
    fclose(fp);
}

/* Delete medicine by id */
void deleteMedicine() {
    printf("\n--- Delete Medicine ---\n");
    printf("Enter medicine ID: ");
    int id; if (scanf("%d", &id) != 1) { printf("Invalid input.\n"); while(getchar()!='\n'); return; }

    FILE *fp = fopen(DATAFILE, "rb");
    if (!fp) { printf("No data file.\n"); return; }
    FILE *tmp = fopen("tmp.dat", "wb");
    if (!tmp) { perror("Unable to create temp file"); fclose(fp); return; }

    Medicine m;
    int found = 0;
    while (fread(&m, sizeof(Medicine), 1, fp) == 1) {
        if (m.id == id) { found = 1; continue; } /* skip writing the deleted record */
        fwrite(&m, sizeof(Medicine), 1, tmp);
    }
    fclose(fp); fclose(tmp);

    if (found) {
        remove(DATAFILE);
        rename("tmp.dat", DATAFILE);
        printf("Medicine with ID %d deleted.\n", id);
    } else {
        remove("tmp.dat");
        printf("Medicine with ID %d not found.\n", id);
    }
}

/* Append sale record to SALESFILE */
void appendSaleRecord(const char *customer_name, CartItem cart[], int cartCount, double subtotal, double tax, double total) {
    FILE *fp = fopen(SALESFILE, "a");
    if (!fp) { perror("Unable to open sales history file"); return; }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestr[64];
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", t);

    fprintf(fp, "Purchase Time: %s\n", timestr);
    if (customer_name && customer_name[0] != '\0')
        fprintf(fp, "Customer: %s\n", customer_name);
    else
        fprintf(fp, "Customer: (not provided)\n");
    fprintf(fp, "Items:\n");
    for (int i = 0; i < cartCount; ++i) {
        double line = cart[i].price * cart[i].qty;
        fprintf(fp, " - %s | ID:%d | Qty:%d | Unit:%.2f | Line:%.2f\n",
                cart[i].name, cart[i].med_id, cart[i].qty, cart[i].price, line);
    }
    fprintf(fp, "Subtotal: %.2f\n", subtotal);
    fprintf(fp, "VAT %.2f%%: %.2f\n", TAX_RATE * 100.0, tax);
    fprintf(fp, "Total: %.2f\n", total);
    fprintf(fp, "----------------------------------------\n");
    fclose(fp);
}

/* Admin view sales history */
void viewSalesHistory() {
    FILE *fp = fopen(SALESFILE, "r");
    if (!fp) { printf("\nNo sales history available.\n"); return; }
    printf("\n--- Sales History ---\n\n");
    int ch;
    while ((ch = fgetc(fp)) != EOF) putchar(ch);
    fclose(fp);
}

/* Admin menu */
void adminMenu() {
    char pass[64];
    printf("\nEnter admin password: ");
    getchar(); /* consume newline */
    fgets(pass, sizeof(pass), stdin);
    pass[strcspn(pass, "\n")] = '\0';
    if (strcmp(pass, ADMIN_PASS) != 0) {
        printf("Incorrect password.\n");
        return;
    }

    int choice;
    do {
        printf("\n--- Admin Panel ---\n");
        printf("1. Add Medicine\n");
        printf("2. View All Medicines\n");
        printf("3. Search Medicine by Name\n");
        printf("4. Update Medicine\n");
        printf("5. Delete Medicine\n");
        printf("6. View Sales History\n");
        printf("0. Back to Main Menu\n");
        printf("Choice: "); if (scanf("%d", &choice) != 1) { while(getchar()!='\n'); choice = -1; }

        switch (choice) {
            case 1: addMedicine(); break;
            case 2: viewMedicines(); break;
            case 3: {
                char keyword[NAME_LEN];
                printf("Enter name keyword: ");
                getchar(); fgets(keyword, NAME_LEN, stdin);
                keyword[strcspn(keyword, "\n")] = '\0';
                searchMedicineByName(keyword);
                break;
            }
            case 4: updateMedicine(); break;
            case 5: deleteMedicine(); break;
            case 6: viewSalesHistory(); break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
        pressEnterToContinue();
    } while (choice != 0);
}

/* Customer purchase flow with add/remove cart and checkout */
void customerMenu() {
    CartItem cart[MAX_CART];
    int cartCount = 0;
    int choice;

    do {
        printf("\n--- Customer Menu ---\n");
        printf("1. Browse all medicines\n");
        printf("2. Search medicine by name\n");
        printf("3. Add medicine to cart (by ID)\n");
        printf("4. Remove item from cart\n");
        printf("5. View cart\n");
        printf("6. Checkout\n");
        printf("0. Back to Main Menu\n");
        printf("Choice: "); if (scanf("%d", &choice) != 1) { while(getchar()!='\n'); choice = -1; }

        if (choice == 1) {
            viewMedicines();
        } else if (choice == 2) {
            char keyword[NAME_LEN];
            printf("Enter name keyword: ");
            getchar(); fgets(keyword, NAME_LEN, stdin);
            keyword[strcspn(keyword, "\n")] = '\0';
            searchMedicineByName(keyword);
        } else if (choice == 3) {
            printf("Enter medicine ID to add: ");
            int id; if (scanf("%d", &id) != 1) { printf("Invalid.\n"); while(getchar()!='\n'); continue; }
            Medicine m;
            if (!searchMedicineByID(id, &m)) { printf("Medicine not found.\n"); continue; }
            if (m.quantity <= 0) { printf("Out of stock.\n"); continue; }
            printf("Available quantity: %d\nEnter desired quantity: ", m.quantity);
            int q; if (scanf("%d", &q) != 1 || q <= 0) { printf("Invalid qty.\n"); while(getchar()!='\n'); continue; }
            if (q > m.quantity) { printf("Only %d units available.\n", m.quantity); continue; }

            /* If already in cart, increase qty */
            int found = 0;
            for (int i=0;i<cartCount;i++){
                if (cart[i].med_id == id) {
                    cart[i].qty += q; found = 1; break;
                }
            }
            if (!found) {
                if (cartCount >= MAX_CART) { printf("Cart is full.\n"); continue; }
                cart[cartCount].med_id = id;
                strncpy(cart[cartCount].name, m.name, NAME_LEN);
                cart[cartCount].price = m.price;
                cart[cartCount].qty = q;
                cartCount++;
            }
            printf("%d x %s added to cart.\n", q, m.name);
        } else if (choice == 4) {
            if (cartCount == 0) { printf("Cart is empty.\n"); continue; }
            printf("\n--- Remove from Cart ---\n");
            for (int i=0;i<cartCount;i++){
                printf("%d) %s | Qty: %d\n", i+1, cart[i].name, cart[i].qty);
            }
            printf("Enter item number to remove (0 to cancel): ");
            int num; if (scanf("%d", &num) != 1) { printf("Invalid.\n"); while(getchar()!='\n'); continue; }
            if (num <= 0) { printf("Cancelled.\n"); continue; }
            if (num > cartCount) { printf("Invalid item number.\n"); continue; }
            /* Remove by shifting */
            for (int i = num - 1; i < cartCount - 1; ++i) cart[i] = cart[i+1];
            cartCount--;
            printf("Item removed from cart.\n");
        } else if (choice == 5) {
            if (cartCount == 0) { printf("Cart is empty.\n"); }
            else {
                printf("\n--- Your Cart ---\n");
                double subtotal = 0.0;
                for (int i=0;i<cartCount;i++){
                    double line = cart[i].price * cart[i].qty;
                    printf("%d) %s | Unit: %.2f | Qty: %d | Line: %.2f\n",
                           i+1, cart[i].name, cart[i].price, cart[i].qty, line);
                    subtotal += line;
                }
                printf("Subtotal: %.2f\n", subtotal);
            }
        } else if (choice == 6) {
            if (cartCount == 0) { printf("Cart empty — add items first.\n"); continue; }
            /* Show invoice */
            printf("\n--- Invoice ---\n");
            double subtotal = 0.0;
            for (int i=0;i<cartCount;i++){
                double line = cart[i].price * cart[i].qty;
                printf("%d) %s | Unit: %.2f | Qty: %d | Line: %.2f\n",
                       i+1, cart[i].name, cart[i].price, cart[i].qty, line);
                subtotal += line;
            }
            double tax = subtotal * TAX_RATE;
            double total = subtotal + tax;
            printf("Subtotal: %.2f\nVAT (%.0f%%): %.2f\nTotal: %.2f\n", subtotal, TAX_RATE*100, tax, total);

            printf("Proceed to payment? (1 = Yes, 0 = No): ");
            int pay; if (scanf("%d", &pay) != 1) { printf("Invalid.\n"); while(getchar()!='\n'); continue; }
            if (pay == 1) {
                /* Optionally ask customer name (can be blank) */
                char customer_name[NAME_LEN];
                printf("Enter your name (press Enter to skip): ");
                getchar(); /* consume newline */
                fgets(customer_name, NAME_LEN, stdin);
                customer_name[strcspn(customer_name, "\n")] = '\0';

                /* Reduce stock and write updated file */
                FILE *fp = fopen(DATAFILE, "rb");
                if (!fp) { printf("Error: data file not found.\n"); continue; }
                FILE *tmp = fopen("tmp.dat", "wb");
                if (!tmp) { printf("Error: cannot open temp file.\n"); fclose(fp); continue; }

                Medicine m;
                int ok = 1;
                while (fread(&m, sizeof(Medicine), 1, fp) == 1) {
                    /* check if in cart */
                    for (int i=0;i<cartCount;i++){
                        if (m.id == cart[i].med_id) {
                            if (cart[i].qty <= m.quantity) {
                                m.quantity -= cart[i].qty;
                            } else {
                                /* Insufficient stock during checkout */
                                printf("Error: insufficient stock for %s during checkout.\n", m.name);
                                ok = 0;
                                break;
                            }
                        }
                    }
                    if (!ok) break;
                    fwrite(&m, sizeof(Medicine), 1, tmp);
                }
                if (!ok) {
                    fclose(fp); fclose(tmp);
                    remove("tmp.dat");
                    printf("Checkout failed due to stock issue. Please adjust cart.\n");
                } else {
                    fclose(fp); fclose(tmp);
                    remove(DATAFILE);
                    rename("tmp.dat", DATAFILE);
                    printf("Payment successful. Thank you for your purchase!\n");
                    /* append sale record */
                    appendSaleRecord(customer_name, cart, cartCount, subtotal, tax, total);
                    /* clear cart */
                    cartCount = 0;
                }
            } else {
                printf("Checkout cancelled.\n");
            }
        } else if (choice == 0) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
        pressEnterToContinue();
    } while (1);
}

/* Main menu */
int main() {
    int choice;
    do {
        printf("\n=== Medical Store Management System ===\n");
        printf("1. Admin Panel\n");
        printf("2. Customer Panel\n");
        printf("0. Exit\n");
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) { while(getchar()!='\n'); choice = -1; }

        switch (choice) {
            case 1: adminMenu(); break;
            case 2: customerMenu(); break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n"); break;
        }
    } while (choice != 0);

    return 0;
}
