// The encryption program in C++ and ASM with a very simple encryption method - it simply adds 1 to the character.
// The encryption method is written in ASM. You will replace this with your allocated version for the assignment.
// In this version parameters are passed via registers (see 'encrypt' for details).
// Filename: "FoCA Assignment 2 Encryption Original with ASM.cpp"
// Last revised Feb 2018 by A.Oram

char EKey = 'b';			// Replace x with your Encryption key.

#define StudentName "Sabeeh UL-Hassan"

#define MAXCHARS 6		// feel free to alter this, but 6 is the minimum

using namespace std;
#include <conio.h>		// for kbhit
#include <windows.h>
#include <string>       // for strings
#include <fstream>		// file I/O
#include <iostream>		// for cin >> and cout <<
#include <iomanip>		// for fancy output
#include "TimeUtils.h"  // for GetTime, GetDate, etc.

#define dollarchar '$'  // string terminator

char OChars[MAXCHARS],
EChars[MAXCHARS],
DChars[MAXCHARS];	// Global Original, Encrypted, Decrypted character strings

					//----------------------------- C++ Functions ----------------------------------------------------------

void get_char(char& a_character)
{
	cin >> a_character;
	while (((a_character < '0') | (a_character > 'z')) && (a_character != dollarchar))
	{
		cout << "Alphanumeric characters only, please try again > ";
		cin >> a_character;
	}
}
//-------------------------------------------------------------------------------------------------------------

void get_original_chars(int& length)
{
	char next_char = ' ';
	length = 0;
	get_char(next_char);

	while ((length < MAXCHARS) && (next_char != dollarchar))
	{
		OChars[length++] = next_char;
		get_char(next_char);
	}
}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINES -------------------------------------------------------------------------

void encrypt_chars(int length, char EKey)
{
	char temp_char;
	int NewLength = length;

	__asm {
		mov  esi, 0 // Zero the counter
		for_loop: // label for: for loop
		mov  al, OChars[esi] // Accessing the current character 
			mov  temp_char, al //Moving the value of al register into temp_char char

							   //-----MAIN ROUTINE------//
			push   eax              // save register values on stack to be safe
			push   ecx
			movzx  ecx, temp_char  // set up registers (Nb this isn't StdCall or Cdecl)
			lea    eax, EKey
			call   encrypt_11              // encrypt the character
			mov    temp_char, al
			pop    ecx        // restore original register values from stack
			pop    eax
			//-----END MAIN ROUTINE------//

			mov  al, temp_char //Moving the value of temp_char into the register al.
			mov  EChars[esi], al //Moving the value of the register al into Echars[esi]

			inc  esi // Increments the ESI register
			cmp  esi, NewLength // compares ESI with NewLength to check if it meets the condition to jump
			jb   for_loop // if ESI is less than NewLength then jump
	}
	return;

	// Inputs: register EAX = 32-bit address of Ekey,
	//					ECX = the character to be encrypted (in the low 8-bit field, CL).

	__asm {

	encrypt_11:
		push edx // pushes the char to stack, this preserves the value of the register. 
			push ebx // pushes the value to stack, this preserves the value of the register. 
			push ecx // pushes the value to stack, this preserves the value of the register.

			and dword ptr[eax], 0x000000FF // Move the 32-bit integer representation of 0x000000FF (255) into the 4 starting at the address in eax.
			add[eax], 0x02 //adds the vale 0x02 to eax.
			ror byte ptr[eax], 2 //Rotate right eax by 2 bytes

			mov edx, [eax] // moves the eax value to edx. 
			pop ebx // restore the original ebx value
			xor ebx, edx // set the contents of the ebx to zero
			ror bl, 1 // Rotate right bl by 1.
			mov eax, ebx // move contents of EBX register into ESI register

			pop ebx // restore the original ebx value
			pop edx // restore the original ebx value

			ret //return to the original location where the subroutine was called.

	}

	//--- End of Assembly code
}
//*** end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINES -------------------------------------------------------------------------
//
void decrypt_chars(int length, char EKey)
{
	char temp_char;						// Character temporary store

	for (int i = 0; i < length; i++)	// Encrypt characters one at a time
	{
		temp_char = EChars[i];			// Get the next char from Original Chars array

		__asm {							//
			push   eax					// write registers to stack
			push   ecx					// write registers to stack

			movzx  ecx, temp_char // Moves the 8bit ASCII characters into the remaining 24 bits in the registers. (8-32=24)
			lea    eax, EKey				// the address of EKey is placed in eax. 
			push eax // pushing the value as parameter
			push ecx // push the value as parameter
			call   decrypt_11			// call the encyption subroutine.

			add   esp, 8               //tidying up the stack, by adding 4 for each push.
			mov    temp_char, al			//copy the temp_char value into al. 


			pop    ecx					// restore registers on stack
			pop    eax					// restore register on stack 

		}




		//(tolower(DChars[i]));
		DChars[i] = temp_char;			// Store encrypted char in the Encrypted Chars arra




	}
	return;
	__asm {

	decrypt_11:
		push edx // pushes the char to stack, this preserves the value of the register. 
			push ebx // pushes the value to stack, this preserves the value of the register. 
			push ecx // pushes the value to stack, this preserves the value of the register. 

			and dword ptr[eax], 0x000000FF // Move the 32-bit integer representation of 0x000000FF (255) into the 4 starting at the address in eax.
			add[eax], 0x02  // adds the value of 0x02 to eax 
			ror byte ptr[eax], 2  // rotating right [eax] by 2 bytes

			mov edx, [eax] // moves the [eax] value to edx
			pop ebx // restoring original ebx value
			rol bl, 1 // rotating left by 1 byte
			xor ebx, edx // masking ebx with value at edx
			mov eax, ebx // moving the result to eax to return to callee

			pop ebx // restore the original ebx value
			pop edx // restore the original ebx value

			ret //return to the original location where the subroutine was called.

	}

	return;
}
//*** end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------






int main(void)
{
	int char_count(0);  // The number of actual characters entered (upto MAXCHARS limit).

	cout << "\nPlease enter upto " << MAXCHARS << " alphanumeric characters:  ";
	get_original_chars(char_count);

	ofstream EDump;
	EDump.open("EncryptDump.txt", ios::app);
	EDump << "\n\nFoCA Encryption program results (" << StudentName << ") Encryption key = '" << EKey << "'";
	EDump << "\nDate: " << GetDate() << "  Time: " << GetTime();

	// Display and save initial string
	cout << "\n\nOriginal string =  " << OChars << "\tHex = ";
	EDump << "\n\nOriginal string =  " << OChars << "\tHex = ";
	for (int i = 0; i < char_count; i++)
	{
		cout << hex << setw(2) << setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";
		EDump << hex << setw(2) << setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";
	};

	//*****************************************************
	// Encrypt the string and display/save the result
	encrypt_chars(char_count, EKey);

	cout << "\n\nEncrypted string = " << EChars << "\tHex = ";
	EDump << "\n\nEncrypted string = " << EChars << "\tHex = ";
	for (int i = 0; i < char_count; i++)
	{
		cout << ((int(EChars[i])) & 0xFF) << "  ";
		EDump << ((int(EChars[i])) & 0xFF) << "  ";
	}

	//*****************************************************
	// Decrypt the encrypted string and display/save the result
	decrypt_chars(char_count, EKey);

	cout << "\n\nDecrypted string = " << DChars << "\tHex = ";
	EDump << "\n\nDecrypted string = " << DChars << "\tHex = ";
	for (int i = 0; i < char_count; i++)
	{
		cout << ((int(DChars[i])) & 0xFF) << "  ";
		EDump << ((int(DChars[i])) & 0xFF) << "  ";
	}
	//*****************************************************

	cout << "\n\n\n";
	EDump << "\n\n-------------------------------------------------------------";
	EDump.close();
	system("PAUSE");
	return (0);


} // end of whole encryption/decryption program --------------------------------------------------------------------


