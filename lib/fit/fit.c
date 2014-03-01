#include <stdio.h>
#include <string.h>

#include "fit_sdk.h"

/**********
 * FIT C
 **********/
const FIT_UINT8 fit_base_type_sizes[FIT_BASE_TYPES] =
{
   sizeof(FIT_ENUM),
   sizeof(FIT_SINT8),
   sizeof(FIT_UINT8),
   sizeof(FIT_SINT16),
   sizeof(FIT_UINT16),
   sizeof(FIT_SINT32),
   sizeof(FIT_UINT32),
   sizeof(FIT_STRING),
   sizeof(FIT_FLOAT32),
   sizeof(FIT_FLOAT64),
   sizeof(FIT_UINT8Z),
   sizeof(FIT_UINT16Z),
   sizeof(FIT_UINT32Z),
   sizeof(FIT_BYTE),
};

const FIT_ENUM fit_enum_invalid = FIT_ENUM_INVALID;
const FIT_SINT8 fit_sint8_invalid = FIT_SINT8_INVALID;
const FIT_UINT8 fit_uint8_invalid = FIT_UINT8_INVALID;
const FIT_SINT16 fit_sint16_invalid = FIT_SINT16_INVALID;
const FIT_UINT16 fit_uint16_invalid = FIT_UINT16_INVALID;
const FIT_SINT32 fit_sint32_invalid = FIT_SINT32_INVALID;
const FIT_UINT32 fit_uint32_invalid = FIT_UINT32_INVALID;
const FIT_STRING fit_string_invalid = FIT_STRING_INVALID;
const FIT_FLOAT32 fit_float32_invalid = FIT_FLOAT32_INVALID;
const FIT_FLOAT64 fit_float64_invalid = FIT_FLOAT64_INVALID;
const FIT_UINT8Z fit_uint8z_invalid = FIT_UINT8Z_INVALID;
const FIT_UINT16Z fit_uint16z_invalid = FIT_UINT16Z_INVALID;
const FIT_UINT32Z fit_uint32z_invalid = FIT_UINT32Z_INVALID;
const FIT_BYTE fit_byte_invalid = FIT_BYTE_INVALID;

const FIT_CONST_UINT8_PTR fit_base_type_invalids[FIT_BASE_TYPES] =
{
   (FIT_CONST_UINT8_PTR)&fit_enum_invalid,
   (FIT_CONST_UINT8_PTR)&fit_sint8_invalid,
   (FIT_CONST_UINT8_PTR)&fit_uint8_invalid,
   (FIT_CONST_UINT8_PTR)&fit_sint16_invalid,
   (FIT_CONST_UINT8_PTR)&fit_uint16_invalid,
   (FIT_CONST_UINT8_PTR)&fit_sint32_invalid,
   (FIT_CONST_UINT8_PTR)&fit_uint32_invalid,
   (FIT_CONST_UINT8_PTR)&fit_string_invalid,
   (FIT_CONST_UINT8_PTR)&fit_float32_invalid,
   (FIT_CONST_UINT8_PTR)&fit_float64_invalid,
   (FIT_CONST_UINT8_PTR)&fit_uint8z_invalid,
   (FIT_CONST_UINT8_PTR)&fit_uint16z_invalid,
   (FIT_CONST_UINT8_PTR)&fit_uint32z_invalid,
   (FIT_CONST_UINT8_PTR)&fit_byte_invalid,
};

///////////////////////////////////////////////////////////////////////
// Returns architecture type.
// Includes runtime check for little or big endian.
// See FIT_MESG_DEF->arch and FIT_ARCH_*.
///////////////////////////////////////////////////////////////////////
FIT_UINT8 Fit_GetArch(void)
{
   const FIT_UINT16 arch = 0x0100;
   return (*(FIT_UINT8 *)&arch);
}

///////////////////////////////////////////////////////////////////////
// Returns message definition corresponding to global message number.
///////////////////////////////////////////////////////////////////////
const FIT_MESG_DEF *Fit_GetMesgDef(FIT_UINT16 global_mesg_num)
{
   FIT_UINT8 index;

   for (index = 0; index < FIT_MESGS; index++)
   {
      if (fit_mesg_defs[index]->global_mesg_num == global_mesg_num)
         return (FIT_MESG_DEF *) fit_mesg_defs[index];
   }

   return (FIT_MESG_DEF *) FIT_NULL;
}

///////////////////////////////////////////////////////////////////////
// Returns the size of message definition.
///////////////////////////////////////////////////////////////////////
FIT_UINT16 Fit_GetMesgDefSize(const FIT_MESG_DEF *mesg_def)
{
   if (mesg_def == FIT_NULL)
      return 0;

   return FIT_STRUCT_OFFSET(fields, FIT_MESG_DEF) + (FIT_UINT16)mesg_def->num_fields * FIT_FIELD_DEF_SIZE;
}

///////////////////////////////////////////////////////////////////////
// Returns the size of message corresponding to global message number.
///////////////////////////////////////////////////////////////////////
FIT_UINT8 Fit_GetMesgSize(FIT_UINT16 global_mesg_num)
{
   const FIT_MESG_DEF *mesg_def;
   FIT_UINT8 field;
   FIT_UINT8 size = 0;

   mesg_def = Fit_GetMesgDef(global_mesg_num);

   if (mesg_def == FIT_NULL)
      return 0;

   for (field = 0; field < mesg_def->num_fields; field++)
   {
      size += mesg_def->fields[FIT_MESG_DEF_FIELD_OFFSET(size, field)];
   }

   return size;
}

///////////////////////////////////////////////////////////////////////
// Initializes message with invalids.
// Returns 1 if successful, otherwise 0.
///////////////////////////////////////////////////////////////////////
FIT_BOOL Fit_InitMesg(const FIT_MESG_DEF *mesg_def, void *mesg)
{
   FIT_UINT8 *mesg_buf = (FIT_UINT8 *) mesg;
   FIT_UINT8 field;

   if (mesg_def == FIT_NULL)
      return FIT_FALSE;

   for (field = 0; field < mesg_def->num_fields; field++)
   {
      FIT_UINT8 base_type_num = mesg_def->fields[FIT_MESG_DEF_FIELD_OFFSET(base_type, field)] & FIT_BASE_TYPE_NUM_MASK;
      FIT_UINT8 base_type_size;
      FIT_UINT8 field_size;

      if (base_type_num >= FIT_BASE_TYPES)
         return FIT_FALSE;

      base_type_size = fit_base_type_sizes[base_type_num];

      for (field_size = 0; field_size < mesg_def->fields[FIT_MESG_DEF_FIELD_OFFSET(size, field)]; field_size += base_type_size)
      {
         memcpy(mesg_buf, fit_base_type_invalids[base_type_num], base_type_size);
         mesg_buf += base_type_size;
      }
   }

   return FIT_TRUE;
}

///////////////////////////////////////////////////////////////////////
// Returns the byte offset of a field in a message.
///////////////////////////////////////////////////////////////////////
FIT_UINT8 Fit_GetFieldOffset(const FIT_MESG_DEF *mesg_def, FIT_UINT8 field_def_num)
{
   FIT_UINT8 offset = 0;
   FIT_UINT8 field;

   if (mesg_def == FIT_NULL)
      return FIT_UINT8_INVALID;

   for (field = 0; field < mesg_def->num_fields; field++)
   {
      if (mesg_def->fields[FIT_MESG_DEF_FIELD_OFFSET(field_def_num, field)] == field_def_num)
         return offset;

      offset += mesg_def->fields[FIT_MESG_DEF_FIELD_OFFSET(size, field)];
   }

   return FIT_UINT8_INVALID;
}

///////////////////////////////////////////////////////////////////////
// Returns the definition for a field in a message.
// field_def_num is set to FIT_FIELD_NUM_INVALID if field is not found in message.
///////////////////////////////////////////////////////////////////////
FIT_FIELD_DEF Fit_GetFieldDef(const FIT_MESG_DEF *mesg_def, FIT_UINT8 field_def_num)
{
   FIT_FIELD_DEF field_def;
   const FIT_UINT8 *field_def_ptr = mesg_def->fields;
   FIT_UINT8 field;

   field_def.field_def_num = FIT_FIELD_NUM_INVALID;

   if (mesg_def != FIT_NULL)
   {
      for (field = 0; field < mesg_def->num_fields; field++)
      {

         if (mesg_def->fields[FIT_MESG_DEF_FIELD_OFFSET(field_def_num, field)] == field_def_num)
         {
            memcpy(&field_def, field_def_ptr, FIT_FIELD_DEF_SIZE);
            break;
         }

         field_def_ptr += FIT_FIELD_DEF_SIZE;
      }
   }

   return field_def;
}

///////////////////////////////////////////////////////////////////////
// Finds the byte offset of a message with the given message index in a FIT file
// Requires a pointer to a function which can read a given number of bytes from a provided offset in a FIT file.
// Returns the local message number if successful, or FIT_UINT8_INVALID if unsuccessful.
///////////////////////////////////////////////////////////////////////
FIT_UINT8 Fit_LookupMessage(FIT_UINT16 global_mesg_num, FIT_UINT16 message_index, FIT_UINT32 *offset, FIT_READ_BYTES_FUNC read_bytes_func, FIT_BOOL read_header)
{
   FIT_UINT16 global_mesg_nums[FIT_MAX_LOCAL_MESGS];
   FIT_UINT8 sizes[FIT_MAX_LOCAL_MESGS];
   FIT_UINT16 current_message_index = FIT_UINT16_INVALID;
   #if defined(FIT_MESSAGE_INDEX)
      FIT_UINT16 message_index_offset = FIT_UINT16_INVALID;
   #endif
   FIT_UINT8 i;

   *offset = 0;

   if (read_header)
   {
      if(read_bytes_func(offset, FIT_STRUCT_OFFSET(header_size, FIT_FILE_HDR), sizeof(FIT_UINT8)) != sizeof(FIT_UINT8))
         return FIT_UINT8_INVALID;
   }

   for (i = 0; i < FIT_MAX_LOCAL_MESGS; i++)
      global_mesg_nums[i] = FIT_UINT16_INVALID;

   while (1)
   {
      FIT_UINT8 header;
      FIT_UINT8 local_mesg_num;

      if (read_bytes_func(&header, *offset, sizeof(header)) != sizeof(header))
         return FIT_UINT8_INVALID;

      *offset += sizeof(header);

      if ((header & (FIT_HDR_TIME_REC_BIT | FIT_HDR_TYPE_DEF_BIT)) == FIT_HDR_TYPE_DEF_BIT)
      {
         FIT_MESG_DEF mesg_def_header;
         FIT_FIELD_DEF field_def;
         #if defined(FIT_MESSAGE_INDEX)
            FIT_UINT16 current_message_index_offset = FIT_UINT16_INVALID;  // Initialize to invalid.  If not found, it will remain invalid.
         #endif
         FIT_UINT8 current_size;
         FIT_UINT8 current_field_def;

         local_mesg_num = header & FIT_HDR_TYPE_MASK;

         if (read_bytes_func(&mesg_def_header, *offset, FIT_MESG_DEF_HEADER_SIZE) != FIT_MESG_DEF_HEADER_SIZE)
            return FIT_UINT8_INVALID;

         *offset += FIT_MESG_DEF_HEADER_SIZE;
         global_mesg_nums[local_mesg_num] = mesg_def_header.global_mesg_num;
         current_size = 0;

         for (current_field_def = 0; current_field_def < mesg_def_header.num_fields; current_field_def++)
         {
            if (read_bytes_func(&field_def, *offset, FIT_FIELD_DEF_SIZE) != FIT_FIELD_DEF_SIZE)
               return FIT_UINT8_INVALID;

            #if defined(FIT_MESSAGE_INDEX)
               if (field_def.field_def_num == FIT_FIELD_NUM_MESSAGE_INDEX)
                  current_message_index_offset = current_size;
            #endif

            current_size += field_def.size;
            *offset += FIT_FIELD_DEF_SIZE;
         }

         sizes[local_mesg_num] = current_size;

         #if defined(FIT_MESSAGE_INDEX)
            if (global_mesg_nums[local_mesg_num] == global_mesg_num)
               message_index_offset = current_message_index_offset;
         #endif
      }
      else
      {
         if (header & FIT_HDR_TIME_REC_BIT)
            local_mesg_num = (header & FIT_HDR_TIME_TYPE_MASK) >> FIT_HDR_TIME_TYPE_SHIFT;
         else
            local_mesg_num = header & FIT_HDR_TYPE_MASK;

         if (global_mesg_nums[local_mesg_num] == global_mesg_num)
         {
            // If the requested message index is invalid, we've found a match.
            if (message_index == FIT_UINT16_INVALID)
               return local_mesg_num;

            #if defined(FIT_MESSAGE_INDEX)
               if (message_index_offset != FIT_UINT16_INVALID)
               {
                  // Read the message index.
                  if (read_bytes_func(&current_message_index, *offset + message_index_offset, sizeof(current_message_index)) != sizeof(current_message_index))
                     return FIT_UINT8_INVALID;
               }
               else
            #endif
            {
               current_message_index++;
            }

            #if defined(FIT_MESSAGE_INDEX)
               if ((message_index & FIT_MESSAGE_INDEX_MASK) == (current_message_index & FIT_MESSAGE_INDEX_MASK))
            #else
               if (message_index == current_message_index)
            #endif
            {
               return local_mesg_num;
            }
         }
         else if (global_mesg_nums[local_mesg_num] == FIT_UINT16_INVALID)
         {
            return FIT_UINT8_INVALID;
         }

         *offset += sizes[local_mesg_num];
      }
   }
}

///////////////////////////////////////////////////////////////////////
// Returns the byte offset of a message within a file.
///////////////////////////////////////////////////////////////////////
FIT_UINT32 Fit_GetFileMesgOffset(const FIT_FILE_DEF *file_def, FIT_UINT16 mesg_num, FIT_UINT16 mesg_index)
{
   FIT_UINT32 offset;
   FIT_UINT16 i = 0;

   while (i < file_def->mesg_count)
   {
      if (mesg_num == file_def->mesgs[i].num)
      {
         offset = file_def->mesgs[i].data_file_offset;
         break;
      }

      i++;
   }

   if (i >= file_def->mesg_count)
      return FIT_UINT32_INVALID;

   // Make sure mesg_index is valid for this mesg num and file definition
   if (mesg_index >= file_def->mesgs[i].count)
      return FIT_UINT32_INVALID;

   i = 1 + Fit_GetMesgSize(mesg_num); // Message header byte + data.

   if (FIT_ALIGNMENT > FIT_HDR_SIZE)
      i += FIT_ALIGNMENT - FIT_HDR_SIZE;

   offset += mesg_index * i;

   return offset;
}

/**********
 * FIT CRC
 **********/

FIT_UINT16 FitCRC_Get16(FIT_UINT16 crc, FIT_UINT8 byte)
{
   static const FIT_UINT16 crc_table[16] =
   {
      0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
      0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
   };
   FIT_UINT16 tmp;

   // compute checksum of lower four bits of byte
   tmp = crc_table[crc & 0xF];
   crc  = (crc >> 4) & 0x0FFF;
   crc  = crc ^ tmp ^ crc_table[byte & 0xF];

   // now compute checksum of upper four bits of byte
   tmp = crc_table[crc & 0xF];
   crc  = (crc >> 4) & 0x0FFF;
   crc  = crc ^ tmp ^ crc_table[(byte >> 4) & 0xF];

   return crc;
}

FIT_UINT16 FitCRC_Update16(FIT_UINT16 crc, const volatile void *data, FIT_UINT8 size)
{
   FIT_BYTE *data_ptr = (FIT_BYTE *)data;

   while (size)
   {
      crc = FitCRC_Get16(crc, *data_ptr);
      data_ptr++;
      size--;
   }

   return crc;
}

FIT_UINT16 FitCRC_Calc16(const volatile void *data, FIT_UINT8 size)
{
   return FitCRC_Update16(0, data, size);
}

/**********
 * FIT CONVERT
 **********/

typedef enum
{
   FIT_CONVERT_CONTINUE = 0,
   FIT_CONVERT_MESSAGE_AVAILABLE,
   FIT_CONVERT_ERROR,
   FIT_CONVERT_END_OF_FILE,
   FIT_CONVERT_PROTOCOL_VERSION_NOT_SUPPORTED,
   FIT_CONVERT_DATA_TYPE_NOT_SUPPORTED
} FIT_CONVERT_RETURN;

typedef enum
{
   FIT_CONVERT_DECODE_FILE_HDR,
   FIT_CONVERT_DECODE_RECORD,
   FIT_CONVERT_DECODE_RESERVED1,
   FIT_CONVERT_DECODE_ARCH,
   FIT_CONVERT_DECODE_GTYPE_1,
   FIT_CONVERT_DECODE_GTYPE_2,
   FIT_CONVERT_DECODE_NUM_FIELD_DEFS,
   FIT_CONVERT_DECODE_FIELD_DEF,
   FIT_CONVERT_DECODE_FIELD_DEF_SIZE,
   FIT_CONVERT_DECODE_FIELD_BASE_TYPE,
   FIT_CONVERT_DECODE_FIELD_DATA
} FIT_CONVERT_DECODE_STATE;

typedef struct
{
   FIT_UINT32 file_bytes_left;
   FIT_UINT32 data_offset;
   #if defined(FIT_CONVERT_TIME_RECORD)
      FIT_UINT32 timestamp;
   #endif
   union
   {
      FIT_FILE_HDR file_hdr;
      FIT_UINT8 mesg[FIT_MESG_SIZE];
   }u;
   FIT_MESG_CONVERT convert_table[FIT_LOCAL_MESGS];
   const FIT_MESG_DEF *mesg_def;
   #if defined(FIT_CONVERT_CHECK_CRC)
      FIT_UINT16 crc;
   #endif
   FIT_CONVERT_DECODE_STATE decode_state;
   FIT_UINT8 mesg_index;
   FIT_UINT8 mesg_sizes[FIT_MAX_LOCAL_MESGS];
   FIT_UINT8 mesg_offset;
   FIT_UINT8 num_fields;
   FIT_UINT8 field_num;
   FIT_UINT8 field_index;
   FIT_UINT8 field_offset;
   #if defined(FIT_CONVERT_TIME_RECORD)
      FIT_UINT8 last_time_offset;
   #endif
} FIT_CONVERT_STATE;

#if !defined(FIT_CONVERT_MULTI_THREAD)
   static FIT_CONVERT_STATE state_struct;
   #define state  (&state_struct)
#endif


///////////////////////////////////////////////////////////////////////
// Initialize the state of the converter to start parsing the file.
///////////////////////////////////////////////////////////////////////
#if defined(FIT_CONVERT_MULTI_THREAD)
   void FitConvert_Init(FIT_CONVERT_STATE *state, FIT_BOOL read_file_header)
#else
   void FitConvert_Init(FIT_BOOL read_file_header)
#endif
{
   state->mesg_offset = 0;
   state->data_offset = 0;

#if defined(FIT_CONVERT_CHECK_CRC)
   state->crc = 0;
#endif
#if defined(FIT_CONVERT_TIME_RECORD)
   state->timestamp = 0;
   state->last_time_offset = 0;
#endif

   if (read_file_header)
   {
      state->file_bytes_left = 3; // Header size byte + CRC.
      state->decode_state = FIT_CONVERT_DECODE_FILE_HDR;
   }
   else
   {
      state->file_bytes_left = 0; // Don't read header or check CRC.
      state->decode_state = FIT_CONVERT_DECODE_RECORD;
   }
}
///////////////////////////////////////////////////////////////////////
// Convert a stream of bytes.
// Parameters:
//    state         Pointer to converter state.
//    data          Pointer to a buffer containing bytes from the file stream.
//    size          Number of bytes in the data buffer.
//
// Returns FIT_CONVERT_CONTINUE when the all bytes in data have
// been decoded successfully and ready to accept next bytes in the
// file stream.  No message is available yet.
// Returns FIT_CONVERT_MESSAGE_AVAILABLE when a message is
// complete.  The message is valid until this function is called
// again.
// Returns FIT_CONVERT_ERROR if a decoding error occurs.
// Returns FIT_CONVERT_END_OF_FILE when the file has been decoded successfully.
///////////////////////////////////////////////////////////////////////
#if defined(FIT_CONVERT_MULTI_THREAD)
   FIT_CONVERT_RETURN FitConvert_Read(FIT_CONVERT_STATE *state, const void *data, FIT_UINT32 size)
#else
   FIT_CONVERT_RETURN FitConvert_Read(const void *data, FIT_UINT32 size)
#endif
{
   while (state->data_offset < size)
   {
      FIT_UINT8 datum = *((FIT_UINT8 *) data + state->data_offset);
      state->data_offset++;

      //printf("fit_convert: 0x%02X - %d\n",datum, state->decode_state);

      if (state->file_bytes_left > 0)
      {
         #if defined(FIT_CONVERT_CHECK_CRC)
            state->crc = FitCRC_Get16(state->crc, datum);
         #endif

         state->file_bytes_left--;

         if (state->file_bytes_left == 1) // CRC low byte.
         {
            if (state->decode_state != FIT_CONVERT_DECODE_RECORD)
               return FIT_CONVERT_ERROR;

            continue; // Next byte.
         }
         else if (state->file_bytes_left == 0) // CRC high byte.
         {
            #if defined(FIT_CONVERT_CHECK_CRC)
               if (state->crc != 0)
                  return FIT_CONVERT_ERROR;
            #endif

            return FIT_CONVERT_END_OF_FILE;
         }
      }

      switch (state->decode_state)
      {
         case FIT_CONVERT_DECODE_FILE_HDR:
            if (state->mesg_offset < FIT_FILE_HDR_SIZE)
               *((FIT_UINT8 *) &state->u.file_hdr + state->mesg_offset) = datum;

            if (state->mesg_offset == 0)
               state->file_bytes_left = state->u.file_hdr.header_size + 2; // Increase to read header and CRC.

            state->mesg_offset++;

            if (state->mesg_offset >= state->u.file_hdr.header_size)
            {
               state->file_bytes_left = *((FIT_UINT8 *) &state->u.file_hdr.data_size);
               state->file_bytes_left |= (FIT_UINT32)*((FIT_UINT8 *) &state->u.file_hdr.data_size + 1) << 8;
               state->file_bytes_left |= (FIT_UINT32)*((FIT_UINT8 *) &state->u.file_hdr.data_size + 2) << 16;
               state->file_bytes_left |= (FIT_UINT32)*((FIT_UINT8 *) &state->u.file_hdr.data_size + 3) << 24;
               state->file_bytes_left += 2; // CRC.

               #if defined(FIT_CONVERT_CHECK_FILE_HDR_DATA_TYPE)
                  if (memcmp(state->u.file_hdr.data_type, ".FIT", 4) != 0)
                     return FIT_CONVERT_DATA_TYPE_NOT_SUPPORTED;
               #endif

               if ((state->u.file_hdr.protocol_version & FIT_PROTOCOL_VERSION_MAJOR_MASK) > (FIT_PROTOCOL_VERSION_MAJOR << FIT_PROTOCOL_VERSION_MAJOR_SHIFT))
                  return FIT_CONVERT_PROTOCOL_VERSION_NOT_SUPPORTED;

               state->decode_state = FIT_CONVERT_DECODE_RECORD;
            }
            break;

         case FIT_CONVERT_DECODE_RECORD:
            if (datum & FIT_HDR_TIME_REC_BIT)
            {
               // This is a message data record with time.
               state->mesg_index = (datum & FIT_HDR_TIME_TYPE_MASK) >> FIT_HDR_TIME_TYPE_SHIFT;

               #if defined(FIT_CONVERT_TIME_RECORD)
                  {
                     FIT_UINT8 time_offset = datum & FIT_HDR_TIME_OFFSET_MASK;
                     state->timestamp += (time_offset - state->last_time_offset) & FIT_HDR_TIME_OFFSET_MASK;
                     state->last_time_offset = time_offset;
                  }
               #endif

               state->decode_state = FIT_CONVERT_DECODE_FIELD_DATA;
            }
            else
            {
               state->mesg_index = datum & FIT_HDR_TYPE_MASK;

               if ((datum & FIT_HDR_TYPE_DEF_BIT) == 0)
               {
                  // This is a message data record.
                  state->decode_state = FIT_CONVERT_DECODE_FIELD_DATA;
               }
               else
               {
                  // This is a message definition record.
                  state->mesg_sizes[state->mesg_index] = 0;
                  state->decode_state = FIT_CONVERT_DECODE_RESERVED1;
               }
            }

            if (state->decode_state == FIT_CONVERT_DECODE_FIELD_DATA)
            {
               if (state->mesg_index < FIT_LOCAL_MESGS)
               {
                  state->mesg_def = Fit_GetMesgDef(state->convert_table[state->mesg_index].global_mesg_num);
                  Fit_InitMesg(state->mesg_def, state->u.mesg);

                  #if defined(FIT_CONVERT_TIME_RECORD)
                     if (datum & FIT_HDR_TIME_REC_BIT)
                     {
                        FIT_UINT8 field_offset = Fit_GetFieldOffset(state->mesg_def, FIT_FIELD_NUM_TIMESTAMP);

                        if (field_offset != FIT_UINT8_INVALID)
                           memcpy(&state->u.mesg[field_offset], &state->timestamp, sizeof(state->timestamp));
                     }
                  #endif
               }

               if (state->mesg_sizes[state->mesg_index] == 0)
                  state->decode_state = FIT_CONVERT_DECODE_RECORD;
            }

            state->mesg_offset = 0; // Reset the message byte count.
            state->field_index = 0;
            state->field_offset = 0;
            break;

         case FIT_CONVERT_DECODE_RESERVED1:
            if (state->mesg_index < FIT_LOCAL_MESGS)
               state->convert_table[state->mesg_index].reserved_1 = datum;

            state->decode_state = FIT_CONVERT_DECODE_ARCH;
            break;

         case FIT_CONVERT_DECODE_ARCH:
            if (state->mesg_index < FIT_LOCAL_MESGS)
               state->convert_table[state->mesg_index].arch = datum;

            state->decode_state = FIT_CONVERT_DECODE_GTYPE_1;
            break;

         case FIT_CONVERT_DECODE_GTYPE_1:
            if (state->mesg_index < FIT_LOCAL_MESGS)
               state->convert_table[state->mesg_index].global_mesg_num = datum;

            state->decode_state = FIT_CONVERT_DECODE_GTYPE_2;
            break;

         case FIT_CONVERT_DECODE_GTYPE_2:
            if (state->mesg_index < FIT_LOCAL_MESGS)
            {
               if ((state->convert_table[state->mesg_index].arch & FIT_ARCH_ENDIAN_MASK) == FIT_ARCH_ENDIAN_BIG)
               {
                  state->convert_table[state->mesg_index].global_mesg_num <<= 8;
                  state->convert_table[state->mesg_index].global_mesg_num |= datum;
               }
               else
               {
                  state->convert_table[state->mesg_index].global_mesg_num |= ((FIT_UINT16) datum << 8);
               }

               state->convert_table[state->mesg_index].num_fields = 0; // Initialize.
               state->mesg_def = Fit_GetMesgDef(state->convert_table[state->mesg_index].global_mesg_num);
            }

            state->decode_state = FIT_CONVERT_DECODE_NUM_FIELD_DEFS;
            break;

         case FIT_CONVERT_DECODE_NUM_FIELD_DEFS:
            state->num_fields = datum;

            if (state->num_fields == 0)
            {
               state->decode_state = FIT_CONVERT_DECODE_RECORD;
               break;
            }

            state->field_index = 0;
            state->decode_state = FIT_CONVERT_DECODE_FIELD_DEF;
            break;

         case FIT_CONVERT_DECODE_FIELD_DEF:
            state->field_num = FIT_FIELD_NUM_INVALID;

            if (state->mesg_index < FIT_LOCAL_MESGS)
            {
               if (state->mesg_def != FIT_NULL)
               {
                  FIT_UINT8 local_field_index;
                  FIT_UINT8 local_field_offset = 0;

                  // Search for the field definition in the local mesg definition.
                  for (local_field_index = 0; local_field_index < state->mesg_def->num_fields; local_field_index++)
                  {
                     FIT_UINT8 field_size = state->mesg_def->fields[FIT_MESG_DEF_FIELD_OFFSET(size, local_field_index)];

                     if (state->mesg_def->fields[FIT_MESG_DEF_FIELD_OFFSET(field_def_num, local_field_index)] == datum)
                     {
                        state->field_num = datum;
                        state->convert_table[state->mesg_index].fields[state->convert_table[state->mesg_index].num_fields].offset_in = state->mesg_offset;
                        state->convert_table[state->mesg_index].fields[state->convert_table[state->mesg_index].num_fields].offset_local = local_field_offset;
                        state->convert_table[state->mesg_index].fields[state->convert_table[state->mesg_index].num_fields].size = field_size;
                        break;
                     }

                     local_field_offset += field_size;
                  }
               }
            }

            state->decode_state = FIT_CONVERT_DECODE_FIELD_DEF_SIZE;
            break;

         case FIT_CONVERT_DECODE_FIELD_DEF_SIZE:
            if (state->mesg_index < FIT_LOCAL_MESGS)
            {
               state->mesg_offset += datum;

               if (state->field_num != FIT_FIELD_NUM_INVALID)
               {
                  if (datum < state->convert_table[state->mesg_index].fields[state->convert_table[state->mesg_index].num_fields].size)
                     state->convert_table[state->mesg_index].fields[state->convert_table[state->mesg_index].num_fields].size = datum;
               }
            }

            state->mesg_sizes[state->mesg_index] += datum;

            state->decode_state = FIT_CONVERT_DECODE_FIELD_BASE_TYPE;
            break;

         case FIT_CONVERT_DECODE_FIELD_BASE_TYPE:
            if (state->field_num != FIT_FIELD_NUM_INVALID)
            {
               state->convert_table[state->mesg_index].fields[state->convert_table[state->mesg_index].num_fields].base_type = datum;
               state->convert_table[state->mesg_index].num_fields++;
            }

            state->field_index++;

            if (state->field_index >= state->num_fields)
               state->decode_state = FIT_CONVERT_DECODE_RECORD;
            else
               state->decode_state = FIT_CONVERT_DECODE_FIELD_DEF;
            break;

         case FIT_CONVERT_DECODE_FIELD_DATA:
            state->mesg_offset++;

            if (state->mesg_offset >= state->mesg_sizes[state->mesg_index])
            {
               state->decode_state = FIT_CONVERT_DECODE_RECORD;
            }

            if (state->mesg_index < FIT_LOCAL_MESGS)
            {
               if ((state->mesg_def != FIT_NULL) && (state->field_index < state->convert_table[state->mesg_index].num_fields))
               {
                  if (state->mesg_offset == (state->convert_table[state->mesg_index].fields[state->field_index].offset_in + state->field_offset + 1))
                  {
                     FIT_UINT8 *field = &state->u.mesg[state->convert_table[state->mesg_index].fields[state->field_index].offset_local];

                     field[state->field_offset] = datum; // Store the incoming byte in the local mesg buffer.
                     state->field_offset++;

                     if (state->field_offset >= state->convert_table[state->mesg_index].fields[state->field_index].size)
                     {
                        if (
                              (state->convert_table[state->mesg_index].fields[state->field_index].base_type & FIT_BASE_TYPE_ENDIAN_FLAG) &&
                              ((state->convert_table[state->mesg_index].arch & FIT_ARCH_ENDIAN_MASK) != (Fit_GetArch() & FIT_ARCH_ENDIAN_MASK))
                           )
                        {
                           FIT_UINT8 type_size;
                           FIT_UINT8 element_size;
                           FIT_UINT8 element;
                           FIT_UINT8 index;

                           index = state->convert_table[state->mesg_index].fields[state->field_index].base_type & FIT_BASE_TYPE_NUM_MASK;

                           if (index >= FIT_BASE_TYPES)
                              return FIT_CONVERT_ERROR;

                           type_size = fit_base_type_sizes[index];
                           element_size = state->convert_table[state->mesg_index].fields[state->field_index].size / type_size;

                           for (element = 0; element < element_size; element++)
                           {
                              for (index = 0; index < (type_size / 2); index++)
                              {
                                 FIT_UINT8 tmp = field[element * type_size + index];
                                 field[element * type_size + index] = field[element * type_size + type_size - 1 - index];
                                 field[element * type_size + type_size - 1 - index] = tmp;
                              }
                           }
                        }

                        // Null terminate last character if multi-byte beyond end of field.
                        if (state->convert_table[state->mesg_index].fields[state->field_index].base_type == FIT_BASE_TYPE_STRING)
                        {
                           FIT_UINT8 length = state->convert_table[state->mesg_index].fields[state->field_index].size;
                           FIT_UINT8 index = 0;

                           while (index < length)
                           {
                              FIT_UINT8 char_size;
                              FIT_UINT8 size_mask = 0x80;

                              if (field[index] & size_mask)
                              {
                                 char_size = 0;

                                 while (field[index] & size_mask) // # of bytes in character = # of MSBits
                                 {
                                    char_size++;
                                    size_mask >>= 1;
                                 }
                              }
                              else
                              {
                                 char_size = 1;
                              }

                              if ((FIT_UINT16)(index + char_size) > length)
                              {
                                 while (index < length)
                                 {
                                    field[index++] = 0;
                                 }
                                 break;
                              }

                              index += char_size;
                           }
                        }

                        state->field_offset = 0; // Reset the offset.
                        state->field_index++; // Move on to the next field.

                        if (state->field_index >= state->convert_table[state->mesg_index].num_fields)
                        {
                           #if defined(FIT_CONVERT_TIME_RECORD)
                              {
                                 FIT_UINT8 timestamp_offset = Fit_GetFieldOffset(state->mesg_def, FIT_FIELD_NUM_TIMESTAMP);

                                 if (timestamp_offset != FIT_UINT8_INVALID)
                                 {
                                    if (*((FIT_UINT32 *)&state->u.mesg[timestamp_offset]) != FIT_DATE_TIME_INVALID)
                                    {
                                       memcpy(&state->timestamp, &state->u.mesg[timestamp_offset], sizeof(state->timestamp));
                                       state->last_time_offset = (FIT_UINT8)(state->timestamp & FIT_HDR_TIME_OFFSET_MASK);
                                    }
                                 }
                              }
                           #endif

                           // We have successfully decoded a mesg.
                           return FIT_CONVERT_MESSAGE_AVAILABLE;
                        }
                     }
                  }
               }
            }
            break;

         default:
            // This shouldn't happen.
            return FIT_CONVERT_ERROR;
      }
   }

   state->data_offset = 0;
   return FIT_CONVERT_CONTINUE;
}

///////////////////////////////////////////////////////////////////////
// Returns the global message number of the decoded message.
///////////////////////////////////////////////////////////////////////
#if defined(FIT_CONVERT_MULTI_THREAD)
   FIT_UINT16 FitConvert_GetMessageNumber(FIT_CONVERT_STATE *state)
#else
   FIT_UINT16 FitConvert_GetMessageNumber(void)
#endif
{
   return state->convert_table[state->mesg_index].global_mesg_num;
}

///////////////////////////////////////////////////////////////////////
// Returns a pointer to the data of the decoded message.
// Copy or cast to FIT_*_MESG structure.
///////////////////////////////////////////////////////////////////////
#if defined(FIT_CONVERT_MULTI_THREAD)
   const FIT_UINT8 *FitConvert_GetMessageData(FIT_CONVERT_STATE *state)
#else
   const FIT_UINT8 *FitConvert_GetMessageData(void)
#endif
{
   return state->u.mesg;
}

///////////////////////////////////////////////////////////////////////
// Restores fields that are not in decoded message from mesg_data.
// Use when modifying an existing file.
///////////////////////////////////////////////////////////////////////
#if defined(FIT_CONVERT_MULTI_THREAD)
   void FitConvert_RestoreFields(FIT_CONVERT_STATE *state, const void *mesg)
#else
   void FitConvert_RestoreFields(const void *mesg)
#endif
{
   FIT_UINT8 offset = 0;
   FIT_UINT8 field_index;
   FIT_UINT8 convert_field;

   if (state->mesg_def == FIT_NULL)
      return;

   for (field_index = 0; field_index < state->mesg_def->num_fields; field_index++)
   {
      for (convert_field=0; convert_field < state->convert_table[state->mesg_index].num_fields; convert_field++)
      {
         if (state->convert_table[state->mesg_index].fields[convert_field].offset_local == offset)
            break;
      }

      if (convert_field == state->convert_table[state->mesg_index].num_fields)
         memcpy(&state->u.mesg[offset], &((FIT_UINT8 *)mesg)[offset], state->mesg_def->fields[FIT_MESG_DEF_FIELD_OFFSET(size, field_index)]);

      offset += state->mesg_def->fields[FIT_MESG_DEF_FIELD_OFFSET(size, field_index)];
   }
}


/**********
 * ENCODE
 **********/

///////////////////////////////////////////////////////////////////////
// Private Variables
///////////////////////////////////////////////////////////////////////

static FIT_UINT16 data_crc;

///////////////////////////////////////////////////////////////////////
// Writes data to the file and updates the data CRC.
///////////////////////////////////////////////////////////////////////
void WriteData(const void *data, FIT_UINT8 data_size, FILE *fp)
{
   FIT_UINT8 offset;

   fwrite(data, 1, data_size, fp);

   for (offset = 0; offset < data_size; offset++)
      data_crc = FitCRC_Get16(data_crc, *((FIT_UINT8 *)data + offset));
}

///////////////////////////////////////////////////////////////////////
// Creates a FIT file. Puts a place-holder for the file header on top of the file.
///////////////////////////////////////////////////////////////////////
void WriteFileHeader(FILE *fp)
{
   FIT_FILE_HDR file_header;

   file_header.header_size = FIT_FILE_HDR_SIZE;
   file_header.profile_version = FIT_PROFILE_VERSION;
   file_header.protocol_version = FIT_PROTOCOL_VERSION;
   memcpy((FIT_UINT8 *)&file_header.data_type, ".FIT", 4);
   fseek (fp , 0 , SEEK_END);
   file_header.data_size = ftell(fp) - FIT_FILE_HDR_SIZE - sizeof(FIT_UINT16);
   file_header.crc = FitCRC_Calc16(&file_header, FIT_STRUCT_OFFSET(crc, FIT_FILE_HDR));

   fseek (fp , 0 , SEEK_SET);
   fwrite((void *)&file_header, 1, FIT_FILE_HDR_SIZE, fp);
}

///////////////////////////////////////////////////////////////////////
// Appends a FIT message definition (including the definition header) to the end of a file.
///////////////////////////////////////////////////////////////////////
void WriteMessageDefinition(FIT_UINT8 local_mesg_number, const void *mesg_def_pointer, FIT_UINT8 mesg_def_size, FILE *fp)
{
   FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT;
   WriteData(&header, FIT_HDR_SIZE, fp);
   WriteData(mesg_def_pointer, mesg_def_size, fp);
}

void WriteMessage(FIT_UINT8 local_mesg_number, const void *mesg_pointer, FIT_UINT8 mesg_size, FILE *fp);
///////////////////////////////////////////////////////////////////////
// Appends a FIT message (including the message header) to the end of a file.
///////////////////////////////////////////////////////////////////////
void WriteMessage(FIT_UINT8 local_mesg_number, const void *mesg_pointer, FIT_UINT8 mesg_size, FILE *fp)
{
   WriteData(&local_mesg_number, FIT_HDR_SIZE, fp);
   WriteData(mesg_pointer, mesg_size, fp);
}

int encode(void)
{
   FILE *fp;

   data_crc = 0;
   fp = fopen("test.fit", "w+b");

   WriteFileHeader(fp);

   // Write file id message.
   {
      FIT_FILE_ID_MESG file_id;
      Fit_InitMesg(fit_mesg_defs[FIT_MESG_FILE_ID], &file_id);
      file_id.type = FIT_FILE_SETTINGS;
      file_id.manufacturer = FIT_MANUFACTURER_GARMIN;
      WriteMessageDefinition(0, fit_mesg_defs[FIT_MESG_FILE_ID], FIT_FILE_ID_MESG_DEF_SIZE, fp);
      WriteMessage(0, &file_id, FIT_FILE_ID_MESG_SIZE, fp);
   }

   // Write user profile message.
   {
      FIT_USER_PROFILE_MESG user_profile;
      Fit_InitMesg(fit_mesg_defs[FIT_MESG_USER_PROFILE], &user_profile);
      user_profile.gender = FIT_GENDER_FEMALE;
      user_profile.age = 35;
      WriteMessageDefinition(0, fit_mesg_defs[FIT_MESG_USER_PROFILE], FIT_USER_PROFILE_MESG_DEF_SIZE, fp);
      WriteMessage(0, &user_profile, FIT_USER_PROFILE_MESG_SIZE, fp);
   }

   // Write CRC.
   fwrite(&data_crc, 1, sizeof(FIT_UINT16), fp);

   // Update file header with data size.
   WriteFileHeader(fp);

   fclose(fp);

   return 0;
}

/**********
 * DECODE
 **********/

int decode(int argc, char* argv[])
{
   FILE *file;
   FIT_UINT8 buf[8];
   FIT_CONVERT_RETURN convert_return = FIT_CONVERT_CONTINUE;
   FIT_UINT32 buf_size;
   FIT_UINT32 mesg_index = 0;
   #if defined(FIT_CONVERT_MULTI_THREAD)
      FIT_CONVERT_STATE state;
   #endif

   printf("Testing file conversion using %s file...\n", argv[1]);

   #if defined(FIT_CONVERT_MULTI_THREAD)
      FitConvert_Init(&state, FIT_TRUE);
   #else
      FitConvert_Init(FIT_TRUE);
   #endif

   if((file = fopen(argv[1], "rb")) == NULL)
   {
      printf("Error opening file %s.\n", argv[1]);
      return FIT_FALSE;
   }

   while(!feof(file) && (convert_return == FIT_CONVERT_CONTINUE))
   {
      for(buf_size=0;(buf_size < sizeof(buf)) && !feof(file); buf_size++)
      {
         buf[buf_size] = getc(file);
      }

      do
      {
         #if defined(FIT_CONVERT_MULTI_THREAD)
            convert_return = FitConvert_Read(&state, buf, buf_size);
         #else
            convert_return = FitConvert_Read(buf, buf_size);
         #endif

         switch (convert_return)
         {
            case FIT_CONVERT_MESSAGE_AVAILABLE:
            {
               #if defined(FIT_CONVERT_MULTI_THREAD)
                  const FIT_UINT8 *mesg = FitConvert_GetMessageData(&state);
                  FIT_UINT16 mesg_num = FitConvert_GetMessageNumber(&state);
               #else
                  const FIT_UINT8 *mesg = FitConvert_GetMessageData();
                  FIT_UINT16 mesg_num = FitConvert_GetMessageNumber();
               #endif

               printf("Mesg %d (%d) - ", mesg_index++, mesg_num);

               switch(mesg_num)
               {
                  case FIT_MESG_NUM_FILE_ID:
                  {
                     const FIT_FILE_ID_MESG *id = (FIT_FILE_ID_MESG *) mesg;
                     printf("File ID: type=%u, number=%u\n", id->type, id->number);
                     break;
                  }

                  case FIT_MESG_NUM_USER_PROFILE:
                  {
                     const FIT_USER_PROFILE_MESG *user_profile = (FIT_USER_PROFILE_MESG *) mesg;
                     printf("User Profile: weight=%0.1fkg\n", user_profile->weight / 10.0f);
                     break;
                  }

                  case FIT_MESG_NUM_ACTIVITY:
                  {
                     const FIT_ACTIVITY_MESG *activity = (FIT_ACTIVITY_MESG *) mesg;
                     printf("Activity: timestamp=%u, type=%u, event=%u, event_type=%u, num_sessions=%u\n", activity->timestamp, activity->type, activity->event, activity->event_type, activity->num_sessions);
                     {
                        FIT_ACTIVITY_MESG old_mesg;
                        old_mesg.num_sessions = 1;
                        #if defined(FIT_CONVERT_MULTI_THREAD)
                           FitConvert_RestoreFields(&state, &old_mesg);
                        #else
                           FitConvert_RestoreFields(&old_mesg);
                        #endif
                        printf("Restored num_sessions=1 - Activity: timestamp=%u, type=%u, event=%u, event_type=%u, num_sessions=%u\n", activity->timestamp, activity->type, activity->event, activity->event_type, activity->num_sessions);
                     }
                     break;
                  }

                  case FIT_MESG_NUM_SESSION:
                  {
                     const FIT_SESSION_MESG *session = (FIT_SESSION_MESG *) mesg;
                     printf("Session: timestamp=%u\n", session->timestamp);
                     break;
                  }

                  case FIT_MESG_NUM_LAP:
                  {
                     const FIT_LAP_MESG *lap = (FIT_LAP_MESG *) mesg;
                     printf("Lap: timestamp=%u\n", lap->timestamp);
                     break;
                  }

                  case FIT_MESG_NUM_RECORD:
                  {
                     const FIT_RECORD_MESG *record = (FIT_RECORD_MESG *) mesg;

                     printf("Record: timestamp=%u", record->timestamp);

                     if (
                           (record->compressed_speed_distance[0] != FIT_BYTE_INVALID) ||
                           (record->compressed_speed_distance[1] != FIT_BYTE_INVALID) ||
                           (record->compressed_speed_distance[2] != FIT_BYTE_INVALID)
                        )
                     {
                        static FIT_UINT32 accumulated_distance16 = 0;
                        static FIT_UINT32 last_distance16 = 0;
                        FIT_UINT16 speed100;
                        FIT_UINT32 distance16;

                        speed100 = record->compressed_speed_distance[0] | ((record->compressed_speed_distance[1] & 0x0F) << 8);
                        printf(", speed = %0.2fm/s", speed100/100.0f);

                        distance16 = (record->compressed_speed_distance[1] >> 4) | (record->compressed_speed_distance[2] << 4);
                        accumulated_distance16 += (distance16 - last_distance16) & 0x0FFF;
                        last_distance16 = distance16;

                        printf(", distance = %0.3fm", accumulated_distance16/16.0f);
                     }

                     printf("\n");
                     break;
                  }

                  case FIT_MESG_NUM_EVENT:
                  {
                     const FIT_EVENT_MESG *event = (FIT_EVENT_MESG *) mesg;
                     printf("Event: timestamp=%u\n", event->timestamp);
                     break;
                  }

                  case FIT_MESG_NUM_DEVICE_INFO:
                  {
                     const FIT_DEVICE_INFO_MESG *device_info = (FIT_DEVICE_INFO_MESG *) mesg;
                     printf("Device Info: timestamp=%u\n", device_info->timestamp);
                     break;
                  }

                  default:
                    printf("Unknown\n");
                    break;
               }
               break;
            }

            default:
               break;
         }
      } while (convert_return == FIT_CONVERT_MESSAGE_AVAILABLE);
   }

   if (convert_return == FIT_CONVERT_ERROR)
   {
      printf("Error decoding file.\n");
      fclose(file);
      return 1;
   }

   if (convert_return == FIT_CONVERT_CONTINUE)
   {
      printf("Unexpected end of file.\n");
      fclose(file);
      return 1;
   }

   if (convert_return == FIT_CONVERT_PROTOCOL_VERSION_NOT_SUPPORTED)
   {
      printf("Protocol version not supported.\n");
      fclose(file);
      return 1;
   }

   if (convert_return == FIT_CONVERT_END_OF_FILE)
      printf("File converted successfully.\n");

   fclose(file);

   return 0;
}


int main(int argc, char* argv[]) {
#ifdef ENCODE
  return encode();
#else
  return decode(argc, argv);
#endif
}







