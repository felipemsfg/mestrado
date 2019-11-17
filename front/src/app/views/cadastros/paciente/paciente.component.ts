import { Component, OnInit, ViewChild, OnDestroy } from '@angular/core';
import {ModalDirective} from 'ngx-bootstrap/modal';
import { Patient } from '../../../models/patient';
import { PatientService } from '../../../services/patient-service';
import { GuidService } from '../../../services/guid-service';
import { Subscription } from 'rxjs';

@Component({
  selector: 'app-paciente',
  templateUrl: './paciente.component.html',
  styleUrls: ['./paciente.component.scss']
})
export class PacienteComponent implements OnInit, OnDestroy {
    @ViewChild('myModal', {static: false}) public myModal: ModalDirective;
    @ViewChild('pacienteModal', {static: false}) public pacienteModal: ModalDirective;

    newPatient: Patient;
    patientCollection: Patient[];

    subscriptions = new Array<Subscription>();
    constructor(private service: PatientService) {

        service.SalvarSuccess$.subscribe(t => {
            this.service.GetAll();
        });

        service.Collection$.subscribe(collection => {
            this.patientCollection = collection;
        });

        service.DeleteSuccess$.subscribe(t => {
            this.service.GetAll();
        });
    }

    ngOnInit() {
        this.service.GetAll();
    }

    ngOnDestroy() {

    }

    FecharModal() {
        this.pacienteModal.hide();
        this.service.GetAll();
    }

    Edit(item: Patient) {
        this.newPatient = item;
        this.pacienteModal.show();
    }

    Delete(id: string) {
        this.service.Delete(id);
    }

    NovoModal() {
        this.newPatient = new Patient();
        this.pacienteModal.show();
    }

    SalvarPaciente() {
        this.service.Salvar(this.newPatient);
        this.pacienteModal.hide();
    }
    onFileChange(event, position) {
        this.readThis(event.target, position);
    }
    readThis(inputValue: any, position): void {
        const file: File = inputValue.files[0];
        const myReader: FileReader = new FileReader();
        myReader.onloadend = (e) => {
            if (position === 'R') {
                this.newPatient.mtaRightImage = myReader.result;
            } else {
                this.newPatient.mtaLeftImage = myReader.result;
            }
        };
        myReader.readAsDataURL(file);
    }
    OpenUploadModal(image) {
        image.value = '';
        image.click();
    }

}
