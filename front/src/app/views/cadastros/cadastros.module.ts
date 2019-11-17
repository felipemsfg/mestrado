// Angular
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { NgModule } from '@angular/core';
import { ModalModule } from 'ngx-bootstrap/modal';

import { PacienteComponent } from './paciente/paciente.component';
import { CadastrosRoutingModule } from './cadastros-routing.module';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    CadastrosRoutingModule,
    ModalModule.forRoot()
  ],
  declarations: [
    PacienteComponent
  ]
})
export class CadastrosModule { }


