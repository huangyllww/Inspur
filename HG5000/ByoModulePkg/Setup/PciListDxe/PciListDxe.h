#ifndef __SETUP_PCI_LIST_DXE_H__
#define __SETUP_PCI_LIST_DXE_H__

// {357F4A0C-ADEA-4e07-8648-735A551D82D4}

#define SETUP_PCI_LIST_FORMSET_GUID \
  { 0x357f4a0c, 0xadea, 0x4e07, { 0x86, 0x48, 0x73, 0x5a, 0x55, 0x1d, 0x82, 0xd4 } }

#define PCI_LIST_FORMSET_CLASS_ID  0x10
#define PCI_LIST_FORM_ID           1  
  
#define LABEL_PCI_LIST_START   0x1234
#define LABEL_PCI_LIST_END     0x1235


extern EFI_GUID gByoSetupPciListFormsetGuid;

#endif
