//
//    Copyright (C) 2011 Sascha Ittner <sascha.ittner@modusoft.de>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

#include "lcec.h"
#include "lcec_generic.h"

void lcec_generic_read(struct lcec_slave *slave, long period);
void lcec_generic_write(struct lcec_slave *slave, long period);

hal_s32_t lcec_generic_read_s32(uint8_t *pd, lcec_generic_pin_t *lcec_hal_data);
hal_u32_t lcec_generic_read_u32(uint8_t *pd, lcec_generic_pin_t *lcec_hal_data);
void lcec_generic_write_s32(uint8_t *pd, lcec_generic_pin_t *lcec_hal_data, hal_s32_t sval);
void lcec_generic_write_u32(uint8_t *pd, lcec_generic_pin_t *lcec_hal_data, hal_u32_t uval);

int lcec_generic_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_generic_pin_t *lcec_hal_data = (lcec_generic_pin_t *) slave->lcec_hal_data;
  int i, j;
  int err;

  // initialize callbacks
  slave->proc_read = lcec_generic_read;
  slave->proc_write = lcec_generic_write;

  // initialize pins
  for (i=0; i < slave->pdo_entry_count; i++, lcec_hal_data++) {
    // PDO mapping
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, lcec_hal_data->pdo_idx, lcec_hal_data->pdo_sidx, &lcec_hal_data->pdo_os, &lcec_hal_data->pdo_bp);

    switch (lcec_hal_data->type) {
      case HAL_BIT:
        if (lcec_hal_data->bitLength == 1) {
          // single bit pin
          err = hal_pin_bit_newf(lcec_hal_data->dir, ((hal_bit_t **) &lcec_hal_data->pin[0]), comp_id, "%s.%s.%s.%s", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
          if (err != 0) {
            rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.%s failed\n", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
            return err;
          }

          *((hal_bit_t *) lcec_hal_data->pin[0]) = 0;
        } else {
          // bit pin array
          for (j=0; j < LCEC_CONF_GENERIC_MAX_SUBPINS && j < lcec_hal_data->bitLength; j++) {
            err = hal_pin_bit_newf(lcec_hal_data->dir, ((hal_bit_t **) &lcec_hal_data->pin[j]), comp_id, "%s.%s.%s.%s-%d", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name, j);
            if (err != 0) {
              rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.%s-%d failed\n", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name, j);
              return err;
            }

            *((hal_bit_t *) lcec_hal_data->pin[j]) = 0;
          }
        }
        break;

      case HAL_S32:
        // check data size
        if (lcec_hal_data->bitLength > 32) {
          rtapi_print_msg(RTAPI_MSG_WARN, LCEC_MSG_PFX "unable to export S32 pin %s.%s.%s.%s: invalid process data bitlen!\n", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
          continue;
        }

        // export pin
        err = hal_pin_s32_newf(lcec_hal_data->dir, ((hal_s32_t **) &lcec_hal_data->pin[0]), comp_id, "%s.%s.%s.%s", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
        if (err != 0) {
          rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.%s failed\n", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
          return err;
        }

        // initialize data
        *((hal_s32_t *) lcec_hal_data->pin[0]) = 0;
        break;

      case HAL_U32:
        // check data size
        if (lcec_hal_data->bitLength > 32) {
          rtapi_print_msg(RTAPI_MSG_WARN, LCEC_MSG_PFX "unable to export U32 pin %s.%s.%s.%s: invalid process data bitlen!\n", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
          continue;
        }

        // export pin
        err = hal_pin_u32_newf(lcec_hal_data->dir, ((hal_u32_t **) &lcec_hal_data->pin[0]), comp_id, "%s.%s.%s.%s", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
        if (err != 0) {
          rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.%s failed\n", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
          return err;
        }

        // initialize data
        *((hal_u32_t *) lcec_hal_data->pin[0]) = 0;
        break;

      case HAL_FLOAT:
        // check data size
        if (lcec_hal_data->bitLength > 32) {
          rtapi_print_msg(RTAPI_MSG_WARN, LCEC_MSG_PFX "unable to export FLOAT pin %s.%s.%s.%s: invalid process data bitlen!\n", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
          continue;
        }

        // export pin
        err = hal_pin_float_newf(lcec_hal_data->dir, ((hal_float_t **) &lcec_hal_data->pin[0]), comp_id, "%s.%s.%s.%s", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
        if (err != 0) {
          rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.%s failed\n", LCEC_MODULE_NAME, master->name, slave->name, lcec_hal_data->name);
          return err;
        }

        // initialize data
        *((hal_float_t *) lcec_hal_data->pin[0]) = 0.0;
        break;

      default:
        rtapi_print_msg(RTAPI_MSG_WARN, LCEC_MSG_PFX "unsupported pin type %d!\n", lcec_hal_data->type);
    }
  }

  return 0;
}

void lcec_generic_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_generic_pin_t *lcec_hal_data = (lcec_generic_pin_t *) slave->lcec_hal_data;
  uint8_t *pd = master->process_data;
  int i, j, offset;
  hal_float_t fval;

  // read data
  for (i=0; i < slave->pdo_entry_count; i++, lcec_hal_data++) {
    // skip wrong direction and uninitialized pins
    if (lcec_hal_data->dir != HAL_OUT || lcec_hal_data->pin[0] == NULL) {
      continue;
    }

    switch (lcec_hal_data->type) {
      case HAL_BIT:
        offset = ((lcec_hal_data->pdo_os << 3) | (lcec_hal_data->pdo_bp & 0x07)) + lcec_hal_data->bitOffset;
        for (j=0; j < LCEC_CONF_GENERIC_MAX_SUBPINS && lcec_hal_data->pin[j] != NULL; j++, offset++) {
          *((hal_bit_t *) lcec_hal_data->pin[j]) = EC_READ_BIT(&pd[offset >> 3], offset & 0x07);
        }
        break;

      case HAL_S32:
        *((hal_s32_t *) lcec_hal_data->pin[0]) = lcec_generic_read_s32(pd, lcec_hal_data);
        break;

      case HAL_U32:
        *((hal_u32_t *) lcec_hal_data->pin[0]) = lcec_generic_read_u32(pd, lcec_hal_data);
        break;

      case HAL_FLOAT:
        if (lcec_hal_data->subType == lcecPdoEntTypeFloatUnsigned) {
          fval = lcec_generic_read_u32(pd, lcec_hal_data);
        } else {
          fval = lcec_generic_read_s32(pd, lcec_hal_data);
        }

        fval *= lcec_hal_data->floatScale;
        fval += lcec_hal_data->floatOffset;
        *((hal_float_t *) lcec_hal_data->pin[0]) = fval;
        break;

      default:
        continue;
    }
  }
}

void lcec_generic_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_generic_pin_t *lcec_hal_data = (lcec_generic_pin_t *) slave->lcec_hal_data;
  uint8_t *pd = master->process_data;
  int i, j, offset;
  hal_float_t fval;

  // write data
  for (i=0; i<slave->pdo_entry_count; i++, lcec_hal_data++) {
    // skip wrong direction and uninitialized pins
    if (lcec_hal_data->dir != HAL_IN || lcec_hal_data->pin[0] == NULL) {
      continue;
    }

    switch (lcec_hal_data->type) {
      case HAL_BIT:
        offset = ((lcec_hal_data->pdo_os << 3) | (lcec_hal_data->pdo_bp & 0x07)) + lcec_hal_data->bitOffset;
        for (j=0; j < LCEC_CONF_GENERIC_MAX_SUBPINS && lcec_hal_data->pin[j] != NULL; j++, offset++) {
          EC_WRITE_BIT(&pd[offset >> 3], offset & 0x07, *((hal_bit_t *) lcec_hal_data->pin[j]));
        }
        break;

      case HAL_S32:
        lcec_generic_write_s32(pd, lcec_hal_data, *((hal_s32_t *) lcec_hal_data->pin[0]));
        break;

      case HAL_U32:
        lcec_generic_write_u32(pd, lcec_hal_data, *((hal_u32_t *) lcec_hal_data->pin[0]));
        break;

      case HAL_FLOAT:
        fval = *((hal_float_t *) lcec_hal_data->pin[0]);
        fval += lcec_hal_data->floatOffset;
        fval *= lcec_hal_data->floatScale;

        if (lcec_hal_data->subType == lcecPdoEntTypeFloatUnsigned) {
          lcec_generic_write_u32(pd, lcec_hal_data, (hal_u32_t) fval);
        } else {
          lcec_generic_write_s32(pd, lcec_hal_data, (hal_s32_t) fval);
        }
        break;

      default:
        continue;
    }
  }
}

hal_s32_t lcec_generic_read_s32(uint8_t *pd, lcec_generic_pin_t *lcec_hal_data) {
  int i, offset;
  hal_s32_t sval;

  if (lcec_hal_data->pdo_bp == 0 && lcec_hal_data->bitOffset == 0) {
    switch (lcec_hal_data->bitLength) {
      case 8:
        return EC_READ_S8(&pd[lcec_hal_data->pdo_os]);
      case 16:
        return EC_READ_S16(&pd[lcec_hal_data->pdo_os]);
      case 32:
        return EC_READ_S32(&pd[lcec_hal_data->pdo_os]);
    }
  }

  offset = ((lcec_hal_data->pdo_os << 3) | (lcec_hal_data->pdo_bp & 0x07)) + lcec_hal_data->bitOffset;
  for (sval=0, i=0; i < lcec_hal_data->bitLength; i++, offset++) {
    if (EC_READ_BIT(&pd[offset >> 3], offset & 0x07)) {
      sval |= (1 << i);
    }
  }
  return sval;
}

hal_u32_t lcec_generic_read_u32(uint8_t *pd, lcec_generic_pin_t *lcec_hal_data) {
  int i, offset;
  hal_u32_t uval;

  if (lcec_hal_data->pdo_bp == 0 && lcec_hal_data->bitOffset == 0) {
    switch (lcec_hal_data->bitLength) {
      case 8:
        return EC_READ_U8(&pd[lcec_hal_data->pdo_os]);
      case 16:
        return EC_READ_U16(&pd[lcec_hal_data->pdo_os]);
      case 32:
        return EC_READ_U32(&pd[lcec_hal_data->pdo_os]);
    }
  }

  offset = ((lcec_hal_data->pdo_os << 3) | (lcec_hal_data->pdo_bp & 0x07)) + lcec_hal_data->bitOffset;
  for (uval=0, i=0; i < lcec_hal_data->bitLength; i++, offset++) {
    if (EC_READ_BIT(&pd[offset >> 3], offset & 0x07)) {
      uval |= (1 << i);
    }
  }
  return uval;
}

void lcec_generic_write_s32(uint8_t *pd, lcec_generic_pin_t *lcec_hal_data, hal_s32_t sval) {
  int i, offset;

  hal_s32_t lim = ((1LL << lcec_hal_data->bitLength) >> 1) - 1LL; 
  if (sval > lim) sval = lim;
  lim = ~lim;
  if (sval < lim) sval = lim;

  if (lcec_hal_data->pdo_bp == 0 && lcec_hal_data->bitOffset == 0) {
    switch (lcec_hal_data->bitLength) {
      case 8:
        EC_WRITE_S8(&pd[lcec_hal_data->pdo_os], sval);
        return;
      case 16:
        EC_WRITE_S16(&pd[lcec_hal_data->pdo_os], sval);
        return;
      case 32:
        EC_WRITE_S32(&pd[lcec_hal_data->pdo_os], sval);
        return;
    }
  }

  offset = ((lcec_hal_data->pdo_os << 3) | (lcec_hal_data->pdo_bp & 0x07)) + lcec_hal_data->bitOffset;
  for (i=0; i < lcec_hal_data->bitLength; i++, offset++) {
    EC_WRITE_BIT(&pd[offset >> 3], offset & 0x07, sval & 1);
    sval >>= 1;
  }
}

void lcec_generic_write_u32(uint8_t *pd, lcec_generic_pin_t *lcec_hal_data, hal_u32_t uval) {
  int i, offset;

  hal_u32_t lim = (1LL << lcec_hal_data->bitLength) - 1LL; 
  if (uval > lim) uval = lim;

  if (lcec_hal_data->pdo_bp == 0 && lcec_hal_data->bitOffset == 0) {
    switch (lcec_hal_data->bitLength) {
      case 8:
        EC_WRITE_U8(&pd[lcec_hal_data->pdo_os], uval);
        return;
      case 16:
        EC_WRITE_U16(&pd[lcec_hal_data->pdo_os], uval);
        return;
      case 32:
        EC_WRITE_U32(&pd[lcec_hal_data->pdo_os], uval);
        return;
    }
  }

  offset = ((lcec_hal_data->pdo_os << 3) | (lcec_hal_data->pdo_bp & 0x07)) + lcec_hal_data->bitOffset;
  for (i=0; i < lcec_hal_data->bitLength; i++, offset++) {
    EC_WRITE_BIT(&pd[offset >> 3], offset & 0x07, uval & 1);
    uval >>= 1;
  }
}

